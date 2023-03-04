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

            utils::assert_can_cast_skill(entity, next_skill_cast.skill, registry);

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
                    outgoing_effects_component.effect_applications.emplace_back(
                        component::effect_application_t{
                            .condition = effect_application.condition,
                            .source_skill = skill_configuration.skill_key,
                            .effect = effect_application.effect,
                            .unique_effect = effect_application.unique_effect,
                            .direction = component::effect_application_t::convert_direction(
                                effect_application.direction),
                            .base_duration_ms = effect_application.base_duration_ms,
                            .num_stacks = effect_application.num_stacks,
                            .num_targets = effect_application.num_targets});
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
                if (skill_configuration.cooldown[0] != 0 &&
                    !(skill_configuration.skill_key == "Weapon Swap" &&
                      registry.any_of<component::bundle_component>(entity))) {
                    utils::put_skill_on_cooldown(utils::get_owner(entity, registry),
                                                 skill_configuration.skill_key,
                                                 registry);
                }
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

        if (!skill_configuration.equip_bundle.empty()) {
            registry.emplace_or_replace<component::bundle_component>(
                entity, component::bundle_component{skill_configuration.equip_bundle});
            spdlog::info("[{}] {}: equipped bundle {}",
                         utils::get_current_tick(registry),
                         utils::get_entity_name(entity, registry),
                         skill_configuration.equip_bundle);
        } else if (skill_configuration.skill_key == "Weapon Swap") {
            if (auto bundle_ptr = registry.try_get<component::bundle_component>(entity);
                bundle_ptr) {
                spdlog::info("[{}] {}: dropped bundle {}",
                             utils::get_current_tick(registry),
                             utils::get_entity_name(entity, registry),
                             bundle_ptr->name);
                registry.remove<component::bundle_component>(entity);
            } else {
                if (!registry.any_of<component::current_weapon_set>(entity)) {
                    throw std::runtime_error("no equipped_weapon_set on entity");
                }
                auto& equipped_weapons = registry.get<component::equipped_weapons>(entity);
                if (equipped_weapons.weapons.size() == 1) {
                    throw std::runtime_error(
                        "cannot weapon swap when there is only 1 weapon set equipped");
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
        registry.view<component::owner_component, component::is_effect_removal>().each(
            [&](const component::owner_component& owner_component,
                component::is_effect_removal& is_effect_removal) {
                if (owner_component.entity != entity) {
                    return;
                }
                auto& effect_removal = is_effect_removal.effect_removal;
                if (effect_removal.condition.only_applies_on_finished_casting &&
                    *effect_removal.condition.only_applies_on_finished_casting &&
                    (!effect_removal.condition.only_applies_on_finished_casting_skill ||
                     *effect_removal.condition.only_applies_on_finished_casting_skill ==
                         skill_configuration.skill_key) &&
                    (!effect_removal.condition.only_applies_on_finished_casting_skill_with_tag ||
                     utils::skill_has_tag(skill_configuration,
                                          *effect_removal.condition
                                               .only_applies_on_finished_casting_skill_with_tag)) &&
                    utils::conditions_satisfied(
                        effect_removal.condition, owner_component.entity, std::nullopt, registry)) {
                    if (effect_removal.effect != actor::effect_t::INVALID) {
                        registry.view<component::is_effect, component::owner_component>().each(
                            [&](entity_t effect_entity,
                                const component::is_effect& is_effect,
                                const component::owner_component& effect_owner) {
                                if (effect_owner.entity == entity &&
                                    is_effect.effect == effect_removal.effect) {
                                    registry.destroy(effect_entity);
                                }
                            });
                    }
                    if (!effect_removal.unique_effect.empty()) {
                        registry.view<component::is_unique_effect, component::owner_component>()
                            .each([&](entity_t unique_effect_entity,
                                      const component::is_unique_effect& is_unique_effect,
                                      const component::owner_component& effect_owner) {
                                if (effect_owner.entity == entity &&
                                    is_unique_effect.unique_effect.unique_effect_key ==
                                        effect_removal.unique_effect) {
                                    registry.destroy(unique_effect_entity);
                                }
                            });
                    }
                }
            });
        registry.view<component::owner_component, component::skill_triggers_component>().each(
            [&](const component::owner_component& owner_component,
                const component::skill_triggers_component& skill_triggers_component) {
                if (owner_component.entity != entity) {
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
        registry.view<component::owner_component, component::unchained_skill_triggers_component>()
            .each([&](const component::owner_component& owner_component,
                      const component::unchained_skill_triggers_component&
                          unchained_skill_triggers_component) {
                if (owner_component.entity != entity) {
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
