#include "apply_strikes_and_effects.hpp"

#include "configuration/skill.hpp"

#include "component/actor/relative_attributes.hpp"
#include "component/counter/is_counter.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_effect.hpp"
#include "component/encounter/encounter_configuration_component.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/skill/is_skill.hpp"

#include "utils/actor_utils.hpp"
#include "utils/condition_utils.hpp"
#include "utils/effect_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/side_effect_utils.hpp"
#include "utils/weapon_utils.hpp"

namespace gw2combat::system {

struct damage_result_t {
    bool is_critical = false;
    double value = 0.0;
};

damage_result_t calculate_damage(
    const configuration::skill_t& skill_configuration,
    double weapon_strength,
    entity_t strike_source_entity,
    const component::relative_attributes& strike_source_relative_attributes,
    entity_t target_entity,
    const component::relative_attributes& target_relative_attributes,
    registry_t& registry) {
    auto& encounter =
        registry.get<component::encounter_configuration_component>(utils::get_singleton_entity())
            .encounter;
    double skill_intrinsic_damage = weapon_strength * skill_configuration.damage_coefficient;

    double critical_chance_multiplier =
        std::min(strike_source_relative_attributes.get(
                     target_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                 1.0);

    bool is_critical = skill_configuration.can_critical_strike &&
                       (critical_chance_multiplier == 1.0 ||
                        utils::check_random_success(100.0 * critical_chance_multiplier));
    double actual_critical_damage_multiplier =
        std::max(strike_source_relative_attributes.get(
                     target_entity, actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER),
                 1.5);

    double critical_strike_multiplier = [&] {
        if (encounter.critical_strike_mode == configuration::critical_strike_mode_t::MEAN) {
            if (!skill_configuration.can_critical_strike) {
                return 1.0;
            }
            return 1.0 + critical_chance_multiplier * (actual_critical_damage_multiplier - 1.0);
        }
        if (encounter.critical_strike_mode ==
            configuration::critical_strike_mode_t::RANDOM_UNIFORM) {
            return is_critical ? actual_critical_damage_multiplier : 1.0;
        }
        throw std::runtime_error("Invalid critical_strike_mode");
    }();

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

    double damage_value = skill_configuration.flat_damage +
                          skill_intrinsic_damage * source_power * critical_strike_multiplier *
                              effective_strike_damage_multiplier / target_armor;
    return damage_result_t{
        .is_critical = is_critical,
        .value = (double)utils::round_down(damage_value),
    };
}

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
                auto& is_skill = registry.get<component::is_skill>(strike.strike.skill_entity);
                auto& skill_configuration = is_skill.skill_configuration;

                auto damage = calculate_damage(skill_configuration,
                                               strike.strike.weapon_strength_roll,
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

                double total_incoming_damage = std::accumulate(
                    incoming_damage.incoming_damage_events.begin(),
                    incoming_damage.incoming_damage_events.end(),
                    0.0,
                    [](double accumulated,
                       const component::incoming_damage_event& incoming_damage_event) {
                        return accumulated + incoming_damage_event.value;
                    });
                spdlog::info(
                    "[{}] skill {} pow {} fero {} prec {} crit% {} crit_mult {} is_crit {} "
                    "ws_roll {} this_dmg {} total_incoming_dmg {}",
                    utils::get_current_tick(registry),
                    skill_configuration.skill_key,
                    strike_source_relative_attributes.get(target_entity, actor::attribute_t::POWER),
                    strike_source_relative_attributes.get(target_entity,
                                                          actor::attribute_t::FEROCITY),
                    strike_source_relative_attributes.get(target_entity,
                                                          actor::attribute_t::PRECISION),
                    strike_source_relative_attributes.get(
                        target_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                    strike_source_relative_attributes.get(
                        target_entity, actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER),
                    damage.is_critical,
                    strike.strike.weapon_strength_roll,
                    damage.value,
                    total_incoming_damage);

                // NOTE: Extreme hack to avoid coding a whole new type of damage just for food.
                //       Implement properly if there are more such instances!
                if (skill_configuration.skill_key == "Lifesteal Proc") {
                    continue;
                }

                auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                    return utils::on_strike_conditions_satisfied(condition,
                                                                 strike_source_entity,
                                                                 target_entity,
                                                                 damage.is_critical,
                                                                 skill_configuration,
                                                                 registry);
                };
                utils::apply_side_effects(registry, strike_source_entity, side_effect_condition_fn);

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
                        "[{}] {}:{} applied {} stacks of {} duration {} unique effect on {} with "
                        "skill {}",
                        utils::get_current_tick(registry),
                        utils::get_entity_name(application_source_entity, registry),
                        utils::get_entity_name(incoming_application.source_entity, registry),
                        application.num_stacks,
                        application.base_duration_ms,
                        application.unique_effect.unique_effect_key,
                        utils::get_entity_name(target_entity, registry),
                        incoming_application.effect_application.source_skill);
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
                        "[{}] {}:{} applied {} stacks of {} duration {} effect on {} with skill {}",
                        utils::get_current_tick(registry),
                        utils::get_entity_name(application_source_entity, registry),
                        utils::get_entity_name(incoming_application.source_entity, registry),
                        application.num_stacks,
                        effective_duration,
                        utils::to_string(application.effect),
                        utils::get_entity_name(target_entity, registry),
                        incoming_application.effect_application.source_skill);

                    auto side_effect_condition_fn =
                        [&](const configuration::condition_t& condition) {
                            return utils::on_effect_application_conditions_satisfied(
                                condition,
                                application_source_entity,
                                target_entity,
                                application.effect,
                                registry);
                        };
                    utils::apply_side_effects(
                        registry, application_source_entity, side_effect_condition_fn);
                }
            }
        });
}

}  // namespace gw2combat::system
