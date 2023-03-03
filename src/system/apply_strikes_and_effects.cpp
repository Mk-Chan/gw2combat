#include "apply_strikes_and_effects.hpp"

#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/team.hpp"
#include "component/counter/is_counter.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/equipment/weapons.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/temporal/cooldown_component.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/gear_utils.hpp"
#include "utils/skill_utils.hpp"

namespace gw2combat::system {

void apply_strikes(registry_t& registry) {
    registry
        .view<component::relative_attributes, component::incoming_strikes_component>(
            entt::exclude<component::owner_component>)
        .each([&](entity_t target_entity,
                  const component::relative_attributes& target_relative_attributes,
                  const component::incoming_strikes_component& incoming_strikes_component) {
            for (const auto& strike : incoming_strikes_component.strikes) {
                entity_t strike_source_entity = utils::get_owner(strike.source_entity, registry);
                auto& strike_source_relative_attributes =
                    registry.get<component::relative_attributes>(strike_source_entity);

                auto& skill_configuration =
                    registry
                        .get<component::skill_configuration_component>(strike.strike.skill_entity)
                        .skill_configuration;

                double skill_intrinsic_damage =
                    utils::get_weapon_strength(
                        strike_source_entity, skill_configuration.weapon_type, registry) *
                    skill_configuration.damage_coefficient;

                double critical_chance_multiplier =
                    std::min(strike_source_relative_attributes.get(
                                 target_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                             1.0);

                // Will be used instead of average critical_damage_multiplier under some
                // configuration
                bool critical_roll =
                    critical_chance_multiplier == 1.0 ||
                    utils::check_random_success(100.0 * critical_chance_multiplier);
                double actual_critical_damage_multiplier =
                    std::max(strike_source_relative_attributes.get(
                                 target_entity, actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER),
                             1.5);

                double average_critical_damage_multiplier =
                    (1.0 +
                     (critical_chance_multiplier * (actual_critical_damage_multiplier - 1.0)));
                if (!skill_configuration.can_critical_strike) {
                    average_critical_damage_multiplier = 1.0;
                }

                double effective_strike_damage_multiplier =
                    strike_source_relative_attributes.get(
                        target_entity, actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER) *
                    (1.0 + strike_source_relative_attributes.get(
                               target_entity,
                               actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP)) *
                    target_relative_attributes.get(
                        strike_source_entity,
                        actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER) *
                    (1.0 + target_relative_attributes.get(
                               strike_source_entity,
                               actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP));

                double source_power =
                    strike_source_relative_attributes.get(target_entity, actor::attribute_t::POWER);
                double target_armor =
                    target_relative_attributes.get(strike_source_entity, actor::attribute_t::ARMOR);

                double final_incoming_damage = skill_intrinsic_damage * source_power *
                                               average_critical_damage_multiplier *
                                               effective_strike_damage_multiplier / target_armor;

                auto& incoming_damage =
                    registry.get_or_emplace<component::incoming_damage>(target_entity);
                incoming_damage.incoming_damage_events.emplace_back(
                    component::incoming_damage_event{utils::get_current_tick(registry),
                                                     strike_source_entity,
                                                     actor::effect_t::INVALID,
                                                     skill_configuration.skill_key,
                                                     final_incoming_damage});

                double total_incoming_damage = std::accumulate(
                    incoming_damage.incoming_damage_events.begin(),
                    incoming_damage.incoming_damage_events.end(),
                    0.0,
                    [](double accumulated,
                       const component::incoming_damage_event& incoming_damage_event) {
                        return accumulated + incoming_damage_event.value;
                    });
                spdlog::info(
                    "[{}] skill {} pow {} strike_mult {} crit_chance {} crit_mult {} this_dmg {} "
                    "total_incoming_dmg {} outgoing_stuff {}",
                    utils::get_current_tick(registry),
                    utils::to_string(skill_configuration.skill_key),
                    strike_source_relative_attributes.get(target_entity, actor::attribute_t::POWER),
                    effective_strike_damage_multiplier,
                    critical_chance_multiplier,
                    average_critical_damage_multiplier,
                    final_incoming_damage,
                    total_incoming_damage,
                    utils::to_string(skill_configuration.on_strike_effect_applications));

                registry.view<component::owner_component, component::is_counter>().each(
                    [&](const component::owner_component& owner_component,
                        component::is_counter& is_counter) {
                        if (owner_component.entity != strike_source_entity) {
                            return;
                        }
                        bool increment_value = true;
                        for (auto& increment_condition :
                             is_counter.counter_configuration.increment_conditions) {
                            if (!(increment_condition.only_applies_on_strikes &&
                                  *increment_condition.only_applies_on_strikes &&
                                  (!increment_condition.only_applies_on_critical_strikes ||
                                   (*increment_condition.only_applies_on_critical_strikes &&
                                    critical_roll)) &&
                                  (!increment_condition.only_applies_on_strikes_by_skill ||
                                   *increment_condition.only_applies_on_strikes_by_skill ==
                                       skill_configuration.skill_key) &&
                                  (!increment_condition.only_applies_on_strikes_by_skill_with_tag ||
                                   utils::skill_has_tag(
                                       skill_configuration,
                                       *increment_condition
                                            .only_applies_on_strikes_by_skill_with_tag)) &&
                                  utils::conditions_satisfied(increment_condition,
                                                              owner_component.entity,
                                                              target_entity,
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
                        if (source_actor.entity != strike_source_entity) {
                            return;
                        }
                        for (auto& skill_trigger : skill_triggers_component.skill_triggers) {
                            if (skill_trigger.condition.only_applies_on_strikes &&
                                *skill_trigger.condition.only_applies_on_strikes &&
                                (!skill_trigger.condition.only_applies_on_critical_strikes ||
                                 (*skill_trigger.condition.only_applies_on_critical_strikes &&
                                  critical_roll)) &&
                                (!skill_trigger.condition.only_applies_on_strikes_by_skill ||
                                 *skill_trigger.condition.only_applies_on_strikes_by_skill ==
                                     skill_configuration.skill_key) &&
                                (!skill_trigger.condition
                                      .only_applies_on_strikes_by_skill_with_tag ||
                                 utils::skill_has_tag(
                                     skill_configuration,
                                     *skill_trigger.condition
                                          .only_applies_on_strikes_by_skill_with_tag)) &&
                                utils::conditions_satisfied(skill_trigger.condition,
                                                            strike_source_entity,
                                                            target_entity,
                                                            registry)) {
                                utils::enqueue_triggered_skill(
                                    strike_source_entity, skill_trigger.skill_key, registry);
                            }
                        }
                    });
                registry
                    .view<component::source_actor, component::unchained_skill_triggers_component>()
                    .each([&](const component::source_actor& source_actor,
                              const component::unchained_skill_triggers_component&
                                  unchained_skill_triggers_component) {
                        if (source_actor.entity != strike_source_entity) {
                            return;
                        }
                        for (auto& skill_trigger :
                             unchained_skill_triggers_component.skill_triggers) {
                            if (skill_trigger.condition.only_applies_on_strikes &&
                                *skill_trigger.condition.only_applies_on_strikes &&
                                (!skill_trigger.condition.only_applies_on_critical_strikes ||
                                 (*skill_trigger.condition.only_applies_on_critical_strikes &&
                                  critical_roll)) &&
                                (!skill_trigger.condition.only_applies_on_strikes_by_skill ||
                                 *skill_trigger.condition.only_applies_on_strikes_by_skill ==
                                     skill_configuration.skill_key) &&
                                (!skill_trigger.condition
                                      .only_applies_on_strikes_by_skill_with_tag ||
                                 utils::skill_has_tag(
                                     skill_configuration,
                                     *skill_trigger.condition
                                          .only_applies_on_strikes_by_skill_with_tag)) &&
                                utils::conditions_satisfied(skill_trigger.condition,
                                                            strike_source_entity,
                                                            target_entity,
                                                            registry)) {
                                utils::enqueue_child_skill(
                                    strike_source_entity,
                                    utils::get_skill_configuration(
                                        skill_trigger.skill_key, strike_source_entity, registry),
                                    registry);
                            }
                        }
                    });

                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(
                        strike_source_entity);
                std::transform(
                    skill_configuration.on_strike_effect_applications.begin(),
                    skill_configuration.on_strike_effect_applications.end(),
                    std::back_inserter(outgoing_effects_component.effect_applications),
                    [&](const configuration::effect_application_t& effect_application) {
                        return component::effect_application_t{
                            .condition = effect_application.condition,
                            .source_skill = skill_configuration.skill_key,
                            .effect = effect_application.effect,
                            .unique_effect = effect_application.unique_effect,
                            .direction = component::effect_application_t::convert_direction(
                                effect_application.direction),
                            .base_duration_ms = effect_application.base_duration_ms,
                            .num_stacks = effect_application.num_stacks,
                            .num_targets = effect_application.num_targets};
                    });
            }
        });
}

void apply_effects(registry_t& registry) {
    registry.view<component::incoming_effects_component>(entt::exclude<component::owner_component>)
        .each([&](entity_t target_entity,
                  const component::incoming_effects_component& incoming_effects_component) {
            for (const auto& incoming_application :
                 incoming_effects_component.effect_applications) {
                const auto& application = incoming_application.effect_application;
                entity_t application_source_entity =
                    utils::get_owner(incoming_application.source_entity, registry);
                auto& application_source_relative_attributes =
                    registry.get<component::relative_attributes>(application_source_entity);
                if (!application.unique_effect.is_invalid()) {
                    utils::add_unique_effect_to_actor(application.unique_effect,
                                                      application.num_stacks,
                                                      application.base_duration_ms,
                                                      application.source_skill,
                                                      application_source_entity,
                                                      target_entity,
                                                      registry);
                    spdlog::info(
                        "[{}] {}:{} applied {} stacks of {} duration {} effect on {}",
                        utils::get_current_tick(registry),
                        utils::get_entity_name(application_source_entity, registry),
                        utils::get_entity_name(incoming_application.source_entity, registry),
                        application.num_stacks,
                        application.base_duration_ms,
                        application.unique_effect.unique_effect_key,
                        utils::get_entity_name(target_entity, registry));
                }
                if (application.effect != actor::effect_t::INVALID) {
                    int effective_duration =
                        utils::get_effective_effect_duration(application.base_duration_ms,
                                                             application.effect,
                                                             application_source_relative_attributes,
                                                             target_entity);
                    utils::add_effect_to_actor(application.effect,
                                               application.num_stacks,
                                               effective_duration,
                                               application.source_skill,
                                               application_source_entity,
                                               target_entity,
                                               registry);
                    spdlog::info(
                        "[{}] {}:{} applied {} stacks of {} duration {} effect on {}",
                        utils::get_current_tick(registry),
                        utils::get_entity_name(application_source_entity, registry),
                        utils::get_entity_name(incoming_application.source_entity, registry),
                        application.num_stacks,
                        effective_duration,
                        utils::to_string(application.effect),
                        utils::get_entity_name(target_entity, registry));
                }
            }
        });
}

}  // namespace gw2combat::system
