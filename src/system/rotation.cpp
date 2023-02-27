#include "rotation.hpp"

#include "component/actor/casting_skill.hpp"
#include "component/actor/finished_casting_skill.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skills_component.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/skill/skill_configuration_component.hpp"
#include "component/skill/skill_trigger_lock.hpp"
#include "component/temporal/animation_component.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"

namespace gw2combat::system {

void perform_rotations(registry_t& registry) {
    registry
        .view<component::rotation_component>(
            entt::exclude<component::animation_component, component::no_more_rotation>)
        .each([&](entity_t entity, component::rotation_component& rotation_component) {
            auto current_tick = utils::get_current_tick(registry);

            if (rotation_component.current_idx >=
                (int)rotation_component.rotation.skill_casts.size()) {
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

            auto& next_skill_cast =
                rotation_component.rotation.skill_casts[rotation_component.current_idx];

            // Make sure this skill can only be cast at or after the time specified in the rotation
            // configuration
            if (current_tick < next_skill_cast.cast_time_ms + rotation_component.tick_offset) {
                return;
            }

            auto skill_entity =
                registry.get<component::skills_component>(entity).find_by(next_skill_cast.skill);

            if (!registry.any_of<component::skill_trigger_lock>(skill_entity)) {
                registry.emplace<component::skill_trigger_lock>(skill_entity);

                auto& skill_cast_duration =
                    registry.get<component::skill_configuration_component>(skill_entity)
                        .skill_configuration.cast_duration;
                registry.emplace<component::animation_component>(
                    entity, component::animation_component{skill_cast_duration, {0, 0}});
                registry.emplace<component::casting_skill>(
                    entity, component::casting_skill{skill_entity, 0, 0});

                spdlog::info("[{}] {} casting skill {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(next_skill_cast.skill));
            }

            rotation_component.current_idx += 1;
        });
}

void perform_skills(registry_t& registry) {
    registry
        .view<component::animation_component, component::casting_skill>(
            entt::exclude<component::finished_casting_skill>)
        .each([&](entity_t entity,
                  const component::animation_component& animation_component,
                  component::casting_skill& casting_skill) {
            double no_quickness_progress_pct = animation_component.duration[0] == 0
                                                   ? 100
                                                   : animation_component.progress[0] * 100 /
                                                         (double)animation_component.duration[0];
            double quickness_progress_pct = animation_component.duration[1] == 0
                                                ? 100
                                                : animation_component.progress[1] * 100 /
                                                      (double)animation_component.duration[1];

            auto& skill_configuration =
                registry.get<component::skill_configuration_component>(casting_skill.skill_entity)
                    .skill_configuration;

            double effective_progress_pct = no_quickness_progress_pct + quickness_progress_pct;
            int effective_tick =
                (int)((double)animation_component.duration[0] * effective_progress_pct / 100.0);

            while (casting_skill.next_pulse_idx <
                       (int)skill_configuration.pulse_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.pulse_on_tick_list[0][casting_skill.next_pulse_idx]) {
                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(entity);
                for (auto& effect_application : skill_configuration.on_pulse_effect_applications) {
                    outgoing_effects_component.effect_applications.emplace_back(effect_application);
                }
                ++casting_skill.next_pulse_idx;
            }

            while (casting_skill.next_strike_idx <
                       (int)skill_configuration.strike_on_tick_list[0].size() &&
                   effective_tick >=
                       skill_configuration.strike_on_tick_list[0][casting_skill.next_strike_idx]) {
                auto& outgoing_strikes_component =
                    registry.get_or_emplace<component::outgoing_strikes_component>(entity);
                auto this_strike = component::strike_t{casting_skill.skill_entity,
                                                       skill_configuration.num_targets};
                outgoing_strikes_component.strikes.emplace_back(this_strike);
                ++casting_skill.next_strike_idx;
            }

            if (effective_progress_pct >= 100) {
                registry.emplace<component::finished_casting_skill>(
                    entity, component::finished_casting_skill{casting_skill.skill_entity});
                utils::put_skill_on_cooldown(
                    utils::get_owner(entity, registry), skill_configuration.skill_key, registry);
                spdlog::info("[{}] {}: finished casting skill {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             utils::to_string(skill_configuration.skill_key));
            }
        });

    registry.view<component::finished_casting_skill>().each([&](entity_t entity,
                                                                const component::
                                                                    finished_casting_skill&
                                                                        finished_casting_skill) {
        registry.remove<component::skill_trigger_lock>(finished_casting_skill.skill_entity);
        auto& skill_configuration =
            registry
                .get<component::skill_configuration_component>(finished_casting_skill.skill_entity)
                .skill_configuration;
        if (skill_configuration.skill_key == "Weapon Swap") {
            if (registry.any_of<component::bundle_component>(entity)) {
                registry.remove<component::bundle_component>(entity);
            } else {
                if (!registry.any_of<component::current_weapon_set>(entity)) {
                    throw std::runtime_error("no equipped_weapon_set on entity");
                }

                auto current_set = registry.get<component::current_weapon_set>(entity).set;
                if (current_set == actor::weapon_set::SET_1) {
                    registry.replace<component::current_weapon_set>(
                        entity, component::current_weapon_set{actor::weapon_set::SET_2});
                } else {
                    registry.replace<component::current_weapon_set>(
                        entity, component::current_weapon_set{actor::weapon_set::SET_1});
                }
            }
        }

        registry.view<component::owner_component, component::is_counter>().each(
            [&](const component::owner_component& owner_component,
                component::is_counter& is_counter) {
                if (owner_component.entity != entity) {
                    return;
                }
                bool increment_value = true;
                for (auto& increment_condition :
                     is_counter.counter_configuration.increment_conditions) {
                    if (!(increment_condition.only_applies_on_finished_casting &&
                          *increment_condition.only_applies_on_finished_casting &&
                          (!increment_condition.only_applies_on_finished_casting_skill ||
                           *increment_condition.only_applies_on_finished_casting_skill ==
                               skill_configuration.skill_key) &&
                          (!increment_condition.only_applies_on_finished_casting_skill_with_tag ||
                           utils::skill_has_tag(
                               skill_configuration,
                               *increment_condition
                                    .only_applies_on_finished_casting_skill_with_tag)) &&
                          utils::conditions_satisfied(increment_condition,
                                                      owner_component.entity,
                                                      std::nullopt,
                                                      registry))) {
                        increment_value = false;
                        break;
                    }
                }
                if (increment_value) {
                    ++is_counter.value;
                }
            });

        registry.view<component::source_actor, component::skill_triggers_component>().each(
            [&](const component::source_actor& source_actor,
                const component::skill_triggers_component& skill_triggers_component) {
                if (source_actor.entity != entity) {
                    return;
                }
                for (auto& skill_trigger : skill_triggers_component.skill_triggers) {
                    if (skill_trigger.condition.only_applies_on_finished_casting &&
                        *skill_trigger.condition.only_applies_on_finished_casting &&
                        (!skill_trigger.condition.only_applies_on_finished_casting_skill ||
                         *skill_trigger.condition.only_applies_on_finished_casting_skill ==
                             skill_configuration.skill_key) &&
                        (!skill_trigger.condition.only_applies_on_finished_casting_skill_with_tag ||
                         utils::skill_has_tag(
                             skill_configuration,
                             *skill_trigger.condition
                                  .only_applies_on_finished_casting_skill_with_tag)) &&
                        utils::conditions_satisfied(
                            skill_trigger.condition, entity, std::nullopt, registry)) {
                        utils::enqueue_triggered_skill(entity, skill_trigger.skill_key, registry);
                    }
                }
            });
        registry.view<component::source_actor, component::unchained_skill_triggers_component>()
            .each([&](const component::source_actor& source_actor,
                      const component::unchained_skill_triggers_component&
                          unchained_skill_triggers_component) {
                if (source_actor.entity != entity) {
                    return;
                }
                for (auto& skill_trigger : unchained_skill_triggers_component.skill_triggers) {
                    if (skill_trigger.condition.only_applies_on_finished_casting &&
                        *skill_trigger.condition.only_applies_on_finished_casting &&
                        (!skill_trigger.condition.only_applies_on_finished_casting_skill ||
                         *skill_trigger.condition.only_applies_on_finished_casting_skill ==
                             skill_configuration.skill_key) &&
                        (!skill_trigger.condition.only_applies_on_finished_casting_skill_with_tag ||
                         utils::skill_has_tag(
                             skill_configuration,
                             *skill_trigger.condition
                                  .only_applies_on_finished_casting_skill_with_tag)) &&
                        utils::conditions_satisfied(
                            skill_trigger.condition, entity, std::nullopt, registry)) {
                        auto& skills_component = registry.get<component::skills_component>(entity);
                        auto& triggered_skill_configuration =
                            registry
                                .get<component::skill_configuration_component>(
                                    skills_component.find_by(skill_trigger.skill_key))
                                .skill_configuration;
                        utils::enqueue_child_skill(entity, triggered_skill_configuration, registry);
                    }
                }
            });

        std::vector<configuration::skill_t> child_skills;
        auto& skills_component =
            registry.get<component::skills_component>(utils::get_owner(entity, registry));
        for (auto&& child_skill_key : skill_configuration.child_skill_keys) {
            auto& child_skill_configuration = registry
                                                  .get<component::skill_configuration_component>(
                                                      skills_component.find_by(child_skill_key))
                                                  .skill_configuration;
            child_skills.emplace_back(child_skill_configuration);
        }
        utils::enqueue_child_skills(entity,
                                    "Temporary " + skill_configuration.skill_key + " Entity",
                                    child_skills,
                                    registry);
    });
}

void cleanup_finished_casting_skills(registry_t& registry) {
    registry.view<component::finished_casting_skill>().each(
        [&](entity_t entity, const component::finished_casting_skill&) {
            registry.remove<component::casting_skill>(entity);
            registry.remove<component::finished_casting_skill>(entity);
        });
}

void destroy_actors_with_no_rotation(registry_t& registry) {
    registry.view<component::destroy_after_rotation, component::no_more_rotation>().each(
        [&](entity_t entity) { registry.destroy(entity); });
}

}  // namespace gw2combat::system
