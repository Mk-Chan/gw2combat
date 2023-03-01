#include "combat_loop.hpp"

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
    actor::rotation_t rotation;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(search_result_t, score, rotation)

struct compact_skill_t {
    actor::skill_t skill;
    int cast_duration;
    int max_ammo;
    int max_cooldown;

    int current_ammo;
    int current_cooldown;
    int last_casted_tick;

    double static_score;
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

tick_t calculate_rotation_cast_time(const configuration::encounter_t& encounter,
                                    const actor::rotation_t& rotation) {
    auto& actor = encounter.actors[0];
    auto build = utils::read<configuration::build_t>(actor.build_path);
    tick_t current = 1;
    for (auto& skill_cast : rotation.skill_casts) {
        for (auto& skill_configuration : build.skills) {
            if (skill_configuration.skill_key != skill_cast.skill) {
                continue;
            }
            current += skill_configuration.cast_duration[1];
        }
    }
    return current;
}

entity_t get_actor_entity_by_name(registry_t& registry, const std::string& name) {
    for (auto entity : registry.view<component::is_actor>()) {
        if (utils::get_entity_name(entity, registry) == name) {
            return entity;
        }
    }
    throw std::runtime_error(fmt::format("no entity with name {} found!", name));
}

tick_t cast_time_for_skill(const configuration::encounter_t& encounter,
                           const actor::skill_t& skill) {
    static auto& actor = encounter.actors[0];
    static auto build = utils::read<configuration::build_t>(actor.build_path);
    for (auto& skill_configuration : build.skills) {
        if (skill_configuration.skill_key != skill) {
            continue;
        }
        return skill_configuration.cast_duration[1];
    }
    throw std::runtime_error("skill configuration not found");
}

double calculate_rotation_score(const configuration::encounter_t& encounter,
                                tick_t horizon,
                                const actor::rotation_t& rotation,
                                tick_t rotation_cast_time) {
    registry_t registry;
    tick_t current_tick = 1;
    registry.ctx().emplace<const tick_t&>(current_tick);

    system::setup_local_encounter(registry, encounter);

    auto player_entity = get_actor_entity_by_name(registry, "slb");
    registry.remove<component::rotation_component>(player_entity);
    registry.emplace_or_replace<component::rotation_component>(
        player_entity, component::rotation_component{rotation, 0, 0, false});

    try {
        // TODO: Save and reuse registry state if this rotation is a subset of a previously
        //       simulated rotation
        while (current_tick < rotation_cast_time + horizon) {
            tick(registry);
            ++current_tick;
        }
        auto& audit =
            registry.get<component::audit_component>(get_actor_entity_by_name(registry, "golem"));
        return std::accumulate(
            audit.incoming_damage_events.cbegin(),
            audit.incoming_damage_events.cend(),
            0.0,
            [&](double accumulated, const component::incoming_damage_event& incoming_damage_event) {
                return incoming_damage_event.source_entity == player_entity
                           ? accumulated + incoming_damage_event.value
                           : accumulated;
            });
    } catch (std::exception& e) {
        spdlog::error("exception:{}", e.what());
        return 0.0;
    }
}

double calculate_skill_damage(const configuration::encounter_t& encounter,
                              const actor::skill_t& skill) {
    actor::rotation_t next_rotation;
    next_rotation.skill_casts.emplace_back(actor::skill_cast_t{skill, 0});
    return calculate_rotation_score(
        encounter, 10'000, next_rotation, cast_time_for_skill(encounter, skill));
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
                             const actor::skill_t& skill) {
    if (skill == "Sic 'Em!") {
        return 100'000;  // Hack to order Sic 'Em! well in the skill list
    }
    return calculate_skill_damage(encounter, skill);
}

skills_state_t new_skills_state(const configuration::encounter_t& encounter) {
    static auto& actor = encounter.actors[0];
    static auto build = utils::read<configuration::build_t>(actor.build_path);
    std::unordered_map<actor::skill_t, compact_skill_t> skill_map;
    for (auto& skill_configuration : build.skills) {
        const auto& skill_key = skill_configuration.skill_key;
        skill_map[skill_key] =
            compact_skill_t{.skill = skill_key,
                            .cast_duration = skill_configuration.cast_duration[1],
                            .max_ammo = skill_configuration.ammo,
                            .max_cooldown = skill_configuration.cooldown[1],
                            .current_ammo = skill_configuration.ammo,
                            .current_cooldown = 0,
                            .last_casted_tick = 0,
                            .static_score = calculate_skill_score(encounter, skill_key)};
    }
    return skills_state_t{.skill_map = skill_map, .latest_skill = "", .current_tick = 1};
}

search_result_t depth_first_search(const configuration::encounter_t& encounter,
                                   const skills_state_t& skills_state,
                                   tick_t horizon,
                                   double best_score,
                                   const actor::rotation_t& prior_rotation,
                                   tick_t prior_rotation_cast_time,
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

    double current_best_score = 0.0;
    actor::rotation_t current_best_rotation;
    for (const actor::skill_t& candidate_skill : sorted_skill_list) {
        actor::rotation_t next_rotation;
        std::copy(prior_rotation.skill_casts.cbegin(),
                  prior_rotation.skill_casts.cend(),
                  std::back_inserter(next_rotation.skill_casts));
        next_rotation.skill_casts.emplace_back(
            actor::skill_cast_t{candidate_skill, prior_rotation_cast_time});
        tick_t next_rotation_cast_time =
            prior_rotation_cast_time + cast_time_for_skill(encounter, candidate_skill);

        search_result_t search_result;
        if (remaining_depth == 1) {
            auto score = calculate_rotation_score(
                encounter, horizon, next_rotation, next_rotation_cast_time);
            search_result = search_result_t{score, next_rotation};
        } else {
            skills_state_t updated_skills_state = skills_state;
            update_skills_state(updated_skills_state, candidate_skill);
            search_result =
                depth_first_search(encounter,
                                   updated_skills_state,
                                   horizon,
                                   best_score,
                                   next_rotation,
                                   next_rotation_cast_time,
                                   remaining_depth - 1,
                                   ply + 1,
                                   candidate_skill == "Weapon Swap" ? weapon ^ 1 : weapon);
        }

        if (search_result.score > current_best_score) {
            current_best_score = search_result.score;
            current_best_rotation = search_result.rotation;

            if (current_best_score > best_score) {
                best_score = current_best_score;
                if (remaining_depth == 1) {
                    spdlog::info("info ply {} best_score {} rotation {}",
                                 ply,
                                 best_score,
                                 utils::to_string(current_best_rotation));
                }
            }
        }
    }

    return search_result_t{current_best_score, current_best_rotation};
}

void search_rotation_for_encounter(const std::string& encounter_configuration_path) {
    auto encounter = utils::read<configuration::encounter_t>(encounter_configuration_path);

    int depth = 15;
    tick_t horizon = 10'000;
    actor::rotation_t prior_rotation;
    auto skills_state = new_skills_state(encounter);
    auto result = depth_first_search(
        encounter, skills_state, horizon, 0.0, prior_rotation, tick_t{0}, depth, 1, 0);
    spdlog::info("{}", utils::to_string(result));
}

}  // namespace gw2combat
