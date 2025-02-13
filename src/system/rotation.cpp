#include "rotation.hpp"

#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_actions_component.hpp"
#include "component/actor/skills_ticks_tracker_component.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_conditional_skill_group.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/animation_component.hpp"

#include "utils/actor_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"
#include "utils/weapon_utils.hpp"

namespace gw2combat::system {

bool perform_rotations(registry_t& registry) {
    bool at_least_one_rotation_performed = false;
    auto& encounter =
        registry.get<component::encounter_configuration_component>(utils::get_singleton_entity())
            .encounter;
    registry.view<component::rotation_component>(entt::exclude<component::no_more_rotation>)
        .each([&](entity_t entity, component::rotation_component& rotation_component) {
            bool already_performed_rotation =
                registry.any_of<component::already_performed_rotation>(entity);
            if (!already_performed_rotation) {
                registry.emplace<component::already_performed_rotation>(entity);
            }

            bool has_queued_rotation = !rotation_component.queued_rotation.empty();
            if (!has_queued_rotation && already_performed_rotation) {
                return;
            }

            auto current_tick = utils::get_current_tick(registry);
            if (rotation_component.current_idx >=
                    static_cast<int>(rotation_component.rotation.skill_casts.size()) &&
                !has_queued_rotation) {
                if (rotation_component.repeat) {
                    rotation_component.current_idx = 0;
                    rotation_component.tick_offset = current_tick;
                } else {
                    registry.emplace<component::no_more_rotation>(entity);
                    spdlog::info("[{}] {} has no more rotation",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry));
                    return;
                }
            }

            auto next_skill_cast =
                has_queued_rotation
                    ? rotation_component.queued_rotation.front()
                    : rotation_component.rotation.skill_casts[rotation_component.current_idx];

            // Make sure this skill can only be cast at or after the time specified in the rotation
            // configuration
            if (current_tick < next_skill_cast.cast_time_ms + rotation_component.tick_offset) {
                return;
            }

            auto skill_entity = utils::get_skill_entity(next_skill_cast.skill, entity, registry);

            auto& skill_configuration =
                registry.get<component::is_skill>(skill_entity).skill_configuration;
            bool is_instant_cast_skill = skill_configuration.cast_duration[0] == 0;
            bool is_in_animation = registry.any_of<component::animation_component>(entity);
            if ((!is_instant_cast_skill ||
                 skill_configuration.instant_cast_only_when_not_in_animation) &&
                is_in_animation) {
                return;
            }
            if (!encounter.require_afk_skills) {
                if (registry.get<component::ammo>(skill_entity).current_ammo <= 0 &&
                    !(skill_configuration.skill_key == "Weapon Swap" &&
                      registry.any_of<component::bundle_component>(entity))) {
                    return;
                }
            }

            at_least_one_rotation_performed = true;

            auto skill_castability = utils::can_cast_skill(skill_entity, registry);
            if (!skill_castability.can_cast) {
                auto cooldown_ptr = registry.try_get<component::cooldown_component>(skill_entity);
                int remaining_cooldown_without_alacrity = 0.0;
                int remaining_cooldown_with_alacrity = 0.0;
                if (cooldown_ptr) {
                    double cooldown_pct = (cooldown_ptr->progress[0] /
                                           static_cast<double>(cooldown_ptr->duration[0])) +
                                          (cooldown_ptr->progress[1] /
                                           static_cast<double>(cooldown_ptr->duration[1]));
                    remaining_cooldown_without_alacrity =
                        static_cast<int>(cooldown_pct * cooldown_ptr->duration[0]);
                    remaining_cooldown_with_alacrity =
                        static_cast<int>(cooldown_pct * cooldown_ptr->duration[1]);
                }
                throw std::runtime_error(
                    fmt::format("[{}] {}: cannot cast skill {}. Reason: {}",
                                utils::get_current_tick(registry),
                                utils::get_entity_name(entity, registry),
                                skill_configuration.skill_key,
                                skill_castability.reason + " (" +
                                    (std::ostringstream() << std::fixed << std::setprecision(1)
                                                          << remaining_cooldown_without_alacrity)
                                        .str() +
                                    ", " +
                                    (std::ostringstream() << std::fixed << std::setprecision(1)
                                                          << remaining_cooldown_with_alacrity)
                                        .str() +
                                    ")"));
            }

            for (auto& skill_to_cancel : skill_configuration.skills_to_cancel) {
                for (auto&& [skill_casting_entity, iter_skills_ticks_tracker_component] :
                     registry.view<component::skills_ticks_tracker_component>().each()) {
                    if (utils::get_owner(skill_casting_entity, registry) !=
                        utils::get_owner(entity, registry)) {
                        continue;
                    }
                    for (auto& iter_skill_state : iter_skills_ticks_tracker_component.skills) {
                        auto& iter_skill_configuration =
                            registry.get<component::is_skill>(iter_skill_state.skill_entity)
                                .skill_configuration;
                        if (iter_skill_configuration.skill_key != skill_to_cancel) {
                            continue;
                        }
                        auto skill_to_cancel_entity = utils::get_skill_entity(
                            skill_to_cancel, skill_casting_entity, registry);
                        auto skill_to_cancel_pos = std::find_if(
                            iter_skills_ticks_tracker_component.skills.cbegin(),
                            iter_skills_ticks_tracker_component.skills.cend(),
                            [&](const component::skills_ticks_tracker_component::skill_state_t&
                                    skill_state) {
                                return skill_state.skill_entity == skill_to_cancel_entity;
                            });
                        if (skill_to_cancel_pos !=
                            iter_skills_ticks_tracker_component.skills.cend()) {
                            spdlog::info(
                                "[{}] {}:{} Canceling skill ticks in skill {} for {}",
                                utils::get_current_tick(registry),
                                utils::get_entity_name(utils::get_owner(entity, registry),
                                                       registry),
                                utils::get_entity_name(entity, registry),
                                skill_to_cancel,
                                utils::get_entity_name(
                                    utils::get_owner(skill_casting_entity, registry), registry));
                            iter_skills_ticks_tracker_component.skills.erase(skill_to_cancel_pos);
                        }
                    }
                }
                for (auto&& [skill_casting_entity, iter_skills_actions_component] :
                     registry.view<component::skills_actions_component>().each()) {
                    if (utils::get_owner(skill_casting_entity, registry) !=
                        utils::get_owner(entity, registry)) {
                        continue;
                    }
                    for (auto& iter_skill_state : iter_skills_actions_component.skills) {
                        auto& iter_skill_configuration =
                            registry.get<component::is_skill>(iter_skill_state.skill_entity)
                                .skill_configuration;
                        if (iter_skill_configuration.skill_key != skill_to_cancel) {
                            continue;
                        }
                        auto skill_to_cancel_entity = utils::get_skill_entity(
                            skill_to_cancel, skill_casting_entity, registry);
                        auto skill_to_cancel_pos = std::find_if(
                            iter_skills_actions_component.skills.cbegin(),
                            iter_skills_actions_component.skills.cend(),
                            [&](const component::skills_actions_component::skill_state_t&
                                    skill_state) {
                                return skill_state.skill_entity == skill_to_cancel_entity;
                            });
                        if (skill_to_cancel_pos != iter_skills_actions_component.skills.cend()) {
                            spdlog::info(
                                "[{}] {}:{} Canceling skill actions in skill {} for {}",
                                utils::get_current_tick(registry),
                                utils::get_entity_name(utils::get_owner(entity, registry),
                                                       registry),
                                utils::get_entity_name(entity, registry),
                                skill_to_cancel,
                                utils::get_entity_name(
                                    utils::get_owner(skill_casting_entity, registry), registry));
                            iter_skills_actions_component.skills.erase(skill_to_cancel_pos);
                        }
                    }
                }
            }

            double weapon_strength_roll = [&] {
                bool any_skill_tick_is_strike =
                    std::any_of(skill_configuration.skill_ticks.begin(),
                                skill_configuration.skill_ticks.end(),
                                [](const configuration::skill_tick_t& skill_tick) {
                                    return skill_tick.strike;
                                });
                bool has_any_legacy_strikes = !skill_configuration.strike_on_tick_list[0].empty() ||
                                              !skill_configuration.strike_on_tick_list[1].empty();
                if (!any_skill_tick_is_strike && !has_any_legacy_strikes) {
                    return 0.0;
                }
                return utils::get_weapon_strength(entity,
                                                  skill_configuration.weapon_type,
                                                  encounter.weapon_strength_mode,
                                                  registry);
            }();

            auto& skills_ticks_tracker_component =
                registry.get_or_emplace<component::skills_ticks_tracker_component>(entity);
            skills_ticks_tracker_component.skills.emplace_back(
                component::skills_ticks_tracker_component::skill_state_t{
                    skill_entity, 0, 0, {{0, weapon_strength_roll}}});

            auto& skills_actions_component =
                registry.get_or_emplace<component::skills_actions_component>(entity);
            skills_actions_component.skills.emplace_back(
                component::skills_actions_component::skill_state_t{
                    skill_entity,
                    {0, 0},
                    0,
                    0,
                    0,
                    weapon_strength_roll,
                });

            auto& begun_casting_skills_component =
                registry.get_or_emplace<component::begun_casting_skills>(entity);
            begun_casting_skills_component.skill_entities.emplace_back(skill_entity);

            if (has_queued_rotation) {
                rotation_component.queued_rotation.pop_front();
            } else {
                rotation_component.current_idx += 1;
            }

            if (is_instant_cast_skill) {
                spdlog::info("[{}] {} casting instant skill {} rotation index {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key),
                             rotation_component.current_idx);
                utils::finish_casting_skill(skill_entity, entity, registry);
            } else {
                registry.emplace<component::animation_component>(
                    entity,
                    component::animation_component{
                        skill_entity, skill_configuration.cast_duration, {0, 0}});
                if (has_queued_rotation) {
                    spdlog::info("[{}] {} casting skill {} queued rotation at index {}",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry),
                                 utils::to_string(skill_configuration.skill_key),
                                 rotation_component.current_idx);
                } else {
                    spdlog::info("[{}] {} casting skill {} rotation index {}",
                                 utils::get_current_tick(registry),
                                 utils::get_entity_name(entity, registry),
                                 utils::to_string(skill_configuration.skill_key),
                                 rotation_component.current_idx);
                }
            }
        });
    return at_least_one_rotation_performed;
}

void destroy_actors_with_no_rotation(registry_t& registry) {
    registry
        .view<component::destroy_after_rotation, component::no_more_rotation>(
            entt::exclude<component::skills_ticks_tracker_component,
                          component::skills_actions_component,
                          component::destroy_skills_ticks_tracker_component,
                          component::finished_skills_actions_component>)
        .each([&](entity_t entity) {
            registry.emplace_or_replace<component::destroy_entity>(entity);
        });
}

}  // namespace gw2combat::system
