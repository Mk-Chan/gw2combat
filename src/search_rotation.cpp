#include "combat_loop.hpp"

#include <iostream>

#include "component/actor/combat_stats.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/audit/audit_component.hpp"

#include "system/encounter.hpp"

#include "utils/entity_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat {

std::array<std::vector<std::string>, 2> allowed_skills_by_weapon{{{
                                                                      "Barrage",
                                                                      "Frost Trap",
                                                                      "Sic 'Em!",
                                                                      "Point-Blank Shot",
                                                                      "One Wolf Pack",
                                                                      "Frenzied Attack",
                                                                      "Worldly Impact",
                                                                      "Weapon Swap",
                                                                      "Rapid Fire",
                                                                      "Long Range Shot",
                                                                      "AFK 50ms",
                                                                  },
                                                                  {
                                                                      "Splitblade",
                                                                      "Frost Trap",
                                                                      "Path of Scars",
                                                                      "Winter's Bite",
                                                                      "Frenzied Attack",
                                                                      "Worldly Impact",
                                                                      "Whirling Defense",
                                                                      "Ricochet",
                                                                      "One Wolf Pack",
                                                                      "Weapon Swap",
                                                                      "Sic 'Em!",
                                                                      "AFK 50ms",
                                                                  }}};

// std::array<std::vector<std::string>, 2> allowed_skills_by_weapon{{{
//                                                                       "Procession of Blades",
//                                                                       "Sword of Justice",
//                                                                       "Spear of Justice",
//                                                                       "Symbol of Energy",
//                                                                       "Deflecting Shot",
//                                                                       "True Shot",
//                                                                       "Hunter's Ward",
//                                                                       "Weapon Swap",
//                                                                       "Puncture Shot",
//                                                                       "Dragon's Maw",
//                                                                   },
//                                                                   {
//                                                                       "Symbol of Resolution",
//                                                                       "Whirling Wrath",
//                                                                       "Leap of Faith",
//                                                                       "Binding Blade",
//                                                                       "Procession of Blades",
//                                                                       "Sword of Justice",
//                                                                       "Spear of Justice",
//                                                                       "Dragon's Maw",
//                                                                       "Strike",
//                                                                       "Vengeful Strike",
//                                                                       "Wrathful Strike",
//                                                                       "Weapon Swap",
//                                                                   }}};

struct search_result_t {
    double score = 0.0;
    configuration::rotation_t rotation;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(search_result_t, score, rotation)

struct compact_skill_t {
    actor::skill_t skill;
    int cast_duration{};
    int max_ammo{};
    int max_cooldown{};

    int current_ammo{};
    int current_cooldown{};
    int last_casted_tick{};

    double static_score{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(compact_skill_t,
                                                skill,
                                                cast_duration,
                                                max_ammo,
                                                max_cooldown,
                                                current_ammo,
                                                current_cooldown,
                                                last_casted_tick,
                                                static_score)

struct skills_state_t {
    std::unordered_map<actor::skill_t, compact_skill_t> skill_map;
    actor::skill_t latest_skill;  // TODO: Implement chain skills properly later
    int current_tick = 1;
};

void print_rotation(const configuration::rotation_t& rotation) {
    for (const auto& skill_cast : rotation.skill_casts) {
        std::cout << "\"" << skill_cast.skill << "\" ";
    }
}

double calculate_rotation_score(const configuration::encounter_t& encounter,
                                const configuration::rotation_t& rotation) {
    configuration::encounter_t next_encounter{encounter};
    next_encounter.actors[0].rotation = rotation;
    next_encounter.termination_conditions.emplace_back(configuration::termination_condition_t{
        configuration::termination_condition_t::type_t::ROTATION, 0, "lb-slb", 0});

    try {
        auto audit = combat_loop(next_encounter, true);
        auto accumulator_visitor = []<typename T>(T&& t) {
            if constexpr (std::is_same_v<std::decay_t<decltype(t)>, audit::damage_event_t>) {
                return t.source_actor == "lb-slb" ? t.damage : 0;
            } else {
                return 0;
            }
        };

        return std::accumulate(audit.tick_events.cbegin(),
                               audit.tick_events.cend(),
                               0.0,
                               [&](double accumulated, const audit::tick_event_t& tick_event) {
                                   if (tick_event.actor == "golem") {
                                       return accumulated +
                                              std::visit(accumulator_visitor, tick_event.event);
                                   }
                                   return accumulated;
                               });
    } catch (std::exception& e) {
        std::cerr << "exception:" << e.what() << std::endl;
        return 0.0;
    }
}

double calculate_skill_damage(const configuration::encounter_t& encounter,
                              const configuration::skill_t& skill) {
    configuration::encounter_t copy_encounter{encounter};
    configuration::rotation_t next_rotation;
    if (skill.weapon_type == actor::weapon_type::AXE) {
        for (auto& weapon : copy_encounter.actors[0].build.weapons) {
            if (weapon.set == actor::weapon_set::SET_1) {
                weapon.set = actor::weapon_set::SET_2;
            } else {
                weapon.set = actor::weapon_set::SET_1;
            }
        }
    }
    next_rotation.skill_casts.emplace_back(skill.skill_key, 0);
    return calculate_rotation_score(copy_encounter, next_rotation);
}

bool is_skill_available_to_cast(const skills_state_t& skills_state, const actor::skill_t& skill) {
    if (skill == "Strike" &&
        (skills_state.latest_skill == "Strike" || skills_state.latest_skill == "Vengeful Strike")) {
        return false;
    }
    if (skill == "Vengeful Strike" && skills_state.latest_skill != "Strike") {
        return false;
    }
    if (skill == "Wrathful Strike" && skills_state.latest_skill != "Vengeful Strike") {
        return false;
    }
    return skills_state.skill_map.at(skill).current_ammo > 0;
}

void update_skills_state(skills_state_t& skills_state, const actor::skill_t& next_skill) {
    auto& current_skill = skills_state.skill_map[next_skill];
    skills_state.current_tick += current_skill.cast_duration;
    skills_state.latest_skill = next_skill;

    if (current_skill.current_ammo == current_skill.max_ammo) {
        current_skill.current_cooldown = current_skill.max_cooldown;
    }
    --current_skill.current_ammo;
    current_skill.last_casted_tick = skills_state.current_tick;

    for (auto& [skill, compact_skill] : skills_state.skill_map) {
        int tick_diff = skills_state.current_tick - compact_skill.last_casted_tick;
        compact_skill.current_cooldown = tick_diff;
        if (tick_diff >= compact_skill.max_cooldown &&
            compact_skill.current_ammo != compact_skill.max_ammo) {
            ++compact_skill.current_ammo;
            compact_skill.current_cooldown =
                compact_skill.current_ammo == compact_skill.max_ammo
                    ? 0
                    : compact_skill.max_cooldown - (tick_diff % compact_skill.max_cooldown);
        }
    }
}

double calculate_skill_score(const configuration::encounter_t& encounter,
                             const configuration::skill_t& skill) {
    if (skill.skill_key == "Long Range Shot") {
        return 0;  // Worst lb-slb skill
    }
    if (skill.skill_key == "One Wolf Pack") {
        return 99'999;  // Hack to order One Wolf Pack well in the skill list
    }
    if (skill.skill_key == "Sic 'Em!") {
        return 100'000;  // Hack to order Sic 'Em! well in the skill list
    }
    if (skill.skill_key == "Barrage") {
        return 99'998;  // Hack to order Barrage well in the skill list
    }
    if (skill.skill_key == "Frost Trap") {
        return 99'997;  // Hack to order Frost Trap well in the skill list
    }

    return calculate_skill_damage(encounter, skill);
}

skills_state_t new_skills_state(const configuration::encounter_t& encounter) {
    static auto& actor = encounter.actors[0];
    std::unordered_map<actor::skill_t, compact_skill_t> skill_map;
    for (auto& skill_configuration : actor.build.skills) {
        const auto& skill_key = skill_configuration.skill_key;
        if (skill_configuration.weapon_type != actor::weapon_type::AXE) {
            auto& lb_allowed_skills = allowed_skills_by_weapon[0];
            if (std::find(lb_allowed_skills.cbegin(), lb_allowed_skills.cend(), skill_key) ==
                lb_allowed_skills.cend()) {
                continue;
            }
        } else if (skill_configuration.weapon_type != actor::weapon_type::LONGBOW) {
            auto& axe_allowed_skills = allowed_skills_by_weapon[1];
            if (std::find(axe_allowed_skills.cbegin(), axe_allowed_skills.cend(), skill_key) ==
                axe_allowed_skills.cend()) {
                continue;
            }
        }
        skill_map[skill_key] =
            compact_skill_t{.skill = skill_key,
                            .cast_duration = skill_configuration.cast_duration[1],
                            .max_ammo = skill_configuration.ammo,
                            .max_cooldown = skill_configuration.cooldown[1],
                            .current_ammo = skill_configuration.ammo,
                            .current_cooldown = 0,
                            .last_casted_tick = 0,
                            .static_score = calculate_skill_score(encounter, skill_configuration)};
    }
    // loop the skill_map in descending order of static_score and print
    std::vector<actor::skill_t> sorted_skill_list;
    for (const auto& [skill, compact_skill] : skill_map) {
        sorted_skill_list.emplace_back(skill);
    }
    std::sort(sorted_skill_list.begin(),
              sorted_skill_list.end(),
              [&](const actor::skill_t& lhs, const actor::skill_t& rhs) {
                  return skill_map.at(lhs).static_score > skill_map.at(rhs).static_score;
              });
    for (const auto& skill : sorted_skill_list) {
        std::cout << "skill " << skill << " static_score " << skill_map[skill].static_score
                  << std::endl;
    }
    return skills_state_t{.skill_map = skill_map, .latest_skill = "", .current_tick = 1};
}

double depth_first_search(const configuration::encounter_t& encounter,
                          const skills_state_t& skills_state,
                          double best_score,
                          configuration::rotation_t& best_rotation,
                          const configuration::rotation_t& prior_rotation,
                          int remaining_depth,
                          int ply,
                          int weapon) {
    std::vector<actor::skill_t> sorted_skill_list;
    for (const actor::skill_t& candidate_skill : allowed_skills_by_weapon[weapon]) {
        if (is_skill_available_to_cast(skills_state, candidate_skill)) {
            sorted_skill_list.emplace_back(candidate_skill);
        }
    }
    std::sort(sorted_skill_list.begin(),
              sorted_skill_list.end(),
              [&](const actor::skill_t& skill1, const actor::skill_t& skill2) {
                  const auto& skill_map = skills_state.skill_map;
                  return (skill_map.at(skill1).static_score) > (skill_map.at(skill2).static_score);
              });

    configuration::rotation_t next_rotation;
    std::copy(prior_rotation.skill_casts.cbegin(),
              prior_rotation.skill_casts.cend(),
              std::back_inserter(next_rotation.skill_casts));
    for (const actor::skill_t& candidate_skill : sorted_skill_list) {
        next_rotation.skill_casts.emplace_back(configuration::skill_cast_t{candidate_skill, 0});

        double search_score;
        if (remaining_depth == 1) {
            search_score = calculate_rotation_score(encounter, next_rotation);
        } else {
            calculate_rotation_score(encounter, next_rotation);

            skills_state_t updated_skills_state = skills_state;
            update_skills_state(updated_skills_state, candidate_skill);
            search_score =
                depth_first_search(encounter,
                                   updated_skills_state,
                                   best_score,
                                   best_rotation,
                                   next_rotation,
                                   remaining_depth - 1,
                                   ply + 1,
                                   candidate_skill == "Weapon Swap" ? weapon ^ 1 : weapon);
        }

        if (search_score > best_score) {
            best_score = search_score;
            best_rotation.skill_casts.clear();
            std::copy(next_rotation.skill_casts.cbegin(),
                      next_rotation.skill_casts.cend(),
                      std::back_inserter(best_rotation.skill_casts));
            if (remaining_depth == 1) {
                std::cout << "info ply " << ply << " best_score " << best_score << " rotation ";
                print_rotation(best_rotation);
                std::cout << std::endl;
            }
        }
        next_rotation.skill_casts.pop_back();
    }

    return best_score;
}

void search_rotation_for_encounter(const configuration::encounter_t& encounter) {
    int max_depth = 15;
    auto skills_state = new_skills_state(encounter);
    configuration::rotation_t best_rotation;
    for (int depth = max_depth; depth <= max_depth; ++depth) {
        configuration::rotation_t prior_rotation;
        auto result = depth_first_search(
            encounter, skills_state, 0.0, best_rotation, prior_rotation, depth, 1, 0);
        std::cout << "info depth " << depth << " best_score " << result << " rotation ";
        print_rotation(best_rotation);
        std::cout << std::endl;
    }
}

}  // namespace gw2combat
