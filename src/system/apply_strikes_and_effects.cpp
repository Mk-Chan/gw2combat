#include "apply_strikes_and_effects.hpp"

#include "configuration/skill.hpp"

#include "component/actor/relative_attributes.hpp"
#include "component/counter/is_counter.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/hierarchy/owner_component.hpp"

#include "component/lifecycle/destroy_entity.hpp"
#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/gear_utils.hpp"
#include "utils/skill_utils.hpp"

namespace gw2combat::system {

struct damage_result_t {
    bool is_critical = false;
    double value = 0.0;
};

damage_result_t calculate_damage(
    const configuration::skill_t& skill_configuration,
    entity_t strike_source_entity,
    const component::relative_attributes& strike_source_relative_attributes,
    entity_t target_entity,
    const component::relative_attributes& target_relative_attributes,
    registry_t& registry) {
    double weapon_strength =
        utils::get_weapon_strength(strike_source_entity, skill_configuration.weapon_type, registry);
    double skill_intrinsic_damage = weapon_strength * skill_configuration.damage_coefficient;

    double critical_chance_multiplier =
        std::min(strike_source_relative_attributes.get(
                     target_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                 1.0);

    // Will be used instead of average critical_damage_multiplier under some configuration
    bool is_critical = critical_chance_multiplier == 1.0 ||
                       utils::check_random_success(100.0 * critical_chance_multiplier);
    double actual_critical_damage_multiplier =
        std::max(strike_source_relative_attributes.get(
                     target_entity, actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER),
                 1.5);

    double average_critical_damage_multiplier =
        (1.0 + (critical_chance_multiplier * (actual_critical_damage_multiplier - 1.0)));
    if (!skill_configuration.can_critical_strike) {
        average_critical_damage_multiplier = 1.0;
    }

    double effective_strike_damage_multiplier =
        strike_source_relative_attributes.get(
            target_entity, actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER) *
        (1.0 +
         strike_source_relative_attributes.get(
             target_entity, actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP)) *
        target_relative_attributes.get(strike_source_entity,
                                       actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER) *
        (1.0 + target_relative_attributes.get(
                   strike_source_entity,
                   actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP));

    double source_power =
        strike_source_relative_attributes.get(target_entity, actor::attribute_t::POWER);
    double target_armor =
        target_relative_attributes.get(strike_source_entity, actor::attribute_t::ARMOR);

    double damage_value = skill_configuration.flat_damage + skill_intrinsic_damage * source_power *
                                                                average_critical_damage_multiplier *
                                                                effective_strike_damage_multiplier /
                                                                target_armor;
    return damage_result_t{
        .is_critical = is_critical,
        .value = damage_value,
    };
}

void apply_strikes(registry_t& registry) {
    std::map<std::tuple<entity_t, std::string>, bool>
        actor_entity_to_skill_trigger_executed_this_tick_flag;
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
                    registry.get<component::is_skill>(strike.strike.skill_entity)
                        .skill_configuration;

                auto damage = calculate_damage(skill_configuration,
                                               strike_source_entity,
                                               strike_source_relative_attributes,
                                               target_entity,
                                               target_relative_attributes,
                                               registry);

                auto& incoming_damage =
                    registry.get_or_emplace<component::incoming_damage>(target_entity);
                incoming_damage.incoming_damage_events.emplace_back(
                    component::incoming_damage_event{utils::get_current_tick(registry),
                                                     strike_source_entity,
                                                     actor::effect_t::INVALID,
                                                     skill_configuration.skill_key,
                                                     damage.value});

                // double total_incoming_damage = std::accumulate(
                //     incoming_damage.incoming_damage_events.begin(),
                //     incoming_damage.incoming_damage_events.end(),
                //     0.0,
                //     [](double accumulated,
                //        const component::incoming_damage_event& incoming_damage_event) {
                //         return accumulated + incoming_damage_event.value;
                //     });
                //  spdlog::info(
                //      "[{}] skill {} pow {} this_dmg {} total_incoming_dmg {}",
                //      utils::get_current_tick(registry),
                //      skill_configuration.skill_key,
                //      strike_source_relative_attributes.get(target_entity,
                //      actor::attribute_t::POWER), damage.value, total_incoming_damage);

                // NOTE: Extreme hack to avoid coding a whole new type of damage just for food.
                //       Implement properly if there are more such instances!
                if (skill_configuration.skill_key == "Lifesteal Proc") {
                    continue;
                }

                registry.view<component::is_counter>().each([&](entity_t counter_entity,
                                                                component::is_counter& is_counter) {
                    auto owner_entity = utils::get_owner(counter_entity, registry);
                    if (owner_entity != strike_source_entity) {
                        return;
                    }
                    bool increment_value = true;
                    for (auto& increment_condition :
                         is_counter.counter_configuration.increment_conditions) {
                        if (!(increment_condition.only_applies_on_strikes &&
                              *increment_condition.only_applies_on_strikes &&
                              (!increment_condition.only_applies_on_critical_strikes ||
                               (*increment_condition.only_applies_on_critical_strikes &&
                                damage.is_critical)) &&
                              (!increment_condition.only_applies_on_strikes_by_skill ||
                               *increment_condition.only_applies_on_strikes_by_skill ==
                                   skill_configuration.skill_key) &&
                              (!increment_condition.only_applies_on_strikes_by_skill_with_tag ||
                               utils::skill_has_tag(
                                   skill_configuration,
                                   *increment_condition
                                        .only_applies_on_strikes_by_skill_with_tag)) &&
                              utils::conditions_satisfied(
                                  increment_condition, owner_entity, target_entity, registry))) {
                            increment_value = false;
                            break;
                        }
                    }
                    if (increment_value) {
                        ++is_counter.value;
                    }
                });
                registry.view<component::is_effect_removal>().each(
                    [&](entity_t effect_removal_entity,
                        component::is_effect_removal& is_effect_removal) {
                        auto owner_entity = utils::get_owner(effect_removal_entity, registry);
                        if (owner_entity != strike_source_entity) {
                            return;
                        }
                        auto& effect_removal = is_effect_removal.effect_removal;
                        if (effect_removal.condition.only_applies_on_strikes &&
                            *effect_removal.condition.only_applies_on_strikes &&
                            (!effect_removal.condition.only_applies_on_critical_strikes ||
                             (*effect_removal.condition.only_applies_on_critical_strikes &&
                              damage.is_critical)) &&
                            (!effect_removal.condition.only_applies_on_strikes_by_skill ||
                             *effect_removal.condition.only_applies_on_strikes_by_skill ==
                                 skill_configuration.skill_key) &&
                            (!effect_removal.condition.only_applies_on_strikes_by_skill_with_tag ||
                             utils::skill_has_tag(
                                 skill_configuration,
                                 *effect_removal.condition
                                      .only_applies_on_strikes_by_skill_with_tag)) &&
                            utils::conditions_satisfied(
                                effect_removal.condition, owner_entity, target_entity, registry)) {
                            if (effect_removal.effect != actor::effect_t::INVALID) {
                                registry.view<component::is_effect, component::owner_component>()
                                    .each([&](entity_t effect_entity,
                                              const component::is_effect& is_effect,
                                              const component::owner_component& effect_owner) {
                                        if (effect_owner.entity == strike_source_entity &&
                                            is_effect.effect == effect_removal.effect) {
                                            registry.emplace_or_replace<component::destroy_entity>(
                                                effect_entity);
                                        }
                                    });
                            }
                            if (!effect_removal.unique_effect.empty()) {
                                registry
                                    .view<component::is_unique_effect, component::owner_component>()
                                    .each([&](entity_t unique_effect_entity,
                                              const component::is_unique_effect& is_unique_effect,
                                              const component::owner_component& effect_owner) {
                                        if (effect_owner.entity == strike_source_entity &&
                                            is_unique_effect.unique_effect.unique_effect_key ==
                                                effect_removal.unique_effect) {
                                            registry.emplace_or_replace<component::destroy_entity>(
                                                unique_effect_entity);
                                        }
                                    });
                            }
                        }
                    });
                registry.view<component::is_skill_trigger>().each([&](entity_t skill_trigger_entity,
                                                                      const component::
                                                                          is_skill_trigger&
                                                                              is_skill_trigger) {
                    auto owner_entity = utils::get_owner(skill_trigger_entity, registry);
                    if (owner_entity != strike_source_entity) {
                        return;
                    }
                    auto& skill_trigger = is_skill_trigger.skill_trigger;
                    if (skill_trigger.condition.only_applies_on_strikes &&
                        *skill_trigger.condition.only_applies_on_strikes &&
                        (!skill_trigger.condition.only_applies_on_critical_strikes ||
                         (*skill_trigger.condition.only_applies_on_critical_strikes &&
                          damage.is_critical)) &&
                        (!skill_trigger.condition.only_applies_on_strikes_by_skill ||
                         *skill_trigger.condition.only_applies_on_strikes_by_skill ==
                             skill_configuration.skill_key) &&
                        (!skill_trigger.condition.only_applies_on_strikes_by_skill_with_tag ||
                         utils::skill_has_tag(
                             skill_configuration,
                             *skill_trigger.condition.only_applies_on_strikes_by_skill_with_tag)) &&
                        utils::conditions_satisfied(
                            skill_trigger.condition, owner_entity, target_entity, registry)) {
                        if (!actor_entity_to_skill_trigger_executed_this_tick_flag[std::make_tuple(
                                owner_entity, skill_trigger.skill_key)]) {
                            actor_entity_to_skill_trigger_executed_this_tick_flag[std::make_tuple(
                                owner_entity, skill_trigger.skill_key)] = true;
                            utils::enqueue_child_skill(
                                skill_trigger.skill_key, strike_source_entity, registry);
                        }
                    }
                });
                registry.view<component::is_unchained_skill_trigger>().each(
                    [&](entity_t skill_trigger_entity,
                        const component::is_unchained_skill_trigger& is_unchained_skill_trigger) {
                        auto owner_entity = utils::get_owner(skill_trigger_entity, registry);
                        if (owner_entity != strike_source_entity) {
                            return;
                        }
                        auto& skill_trigger = is_unchained_skill_trigger.skill_trigger;
                        if (skill_trigger.condition.only_applies_on_strikes &&
                            *skill_trigger.condition.only_applies_on_strikes &&
                            (!skill_trigger.condition.only_applies_on_critical_strikes ||
                             (*skill_trigger.condition.only_applies_on_critical_strikes &&
                              damage.is_critical)) &&
                            (!skill_trigger.condition.only_applies_on_strikes_by_skill ||
                             *skill_trigger.condition.only_applies_on_strikes_by_skill ==
                                 skill_configuration.skill_key) &&
                            (!skill_trigger.condition.only_applies_on_strikes_by_skill_with_tag ||
                             utils::skill_has_tag(
                                 skill_configuration,
                                 *skill_trigger.condition
                                      .only_applies_on_strikes_by_skill_with_tag)) &&
                            utils::conditions_satisfied(
                                skill_trigger.condition, owner_entity, target_entity, registry)) {
                            utils::enqueue_child_skill(
                                skill_trigger.skill_key, strike_source_entity, registry);
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
