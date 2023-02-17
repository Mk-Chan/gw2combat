#include "apply_strikes_and_effects.hpp"

#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/strike_counter.hpp"
#include "component/actor/team.hpp"
#include "component/actor/unique_effects_component.hpp"
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
                    utils::get_weapon_strength(skill_configuration.weapon_type) *
                    skill_configuration.damage_coefficient;

                double critical_chance_multiplier =
                    std::min(strike_source_relative_attributes.get(
                                 target_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                             1.0);
                double actual_critical_damage_multiplier =
                    std::min(strike_source_relative_attributes.get(
                                 target_entity, actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER),
                             1.0);
                double average_critical_damage_multiplier =
                    1.0 + (critical_chance_multiplier * (actual_critical_damage_multiplier - 1.0));

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
                incoming_damage.value += final_incoming_damage;
                spdlog::info(
                    "[{}] skill {} pow {} strike_mult {} crit_mult {} this_dmg {} "
                    "total_incoming_dmg {} outgoing_stuff {}",
                    utils::get_current_tick(registry),
                    utils::to_string(skill_configuration.skill_key),
                    strike_source_relative_attributes.get(target_entity, actor::attribute_t::POWER),
                    effective_strike_damage_multiplier,
                    average_critical_damage_multiplier,
                    final_incoming_damage,
                    incoming_damage.value,
                    utils::to_string(skill_configuration.on_strike_effect_applications));

                if (registry.any_of<component::strike_counter>(strike_source_entity)) {
                    auto& strike_counter =
                        registry.get<component::strike_counter>(strike_source_entity);
                    ++strike_counter.value;
                }

                registry.view<component::owner_component, component::skill_triggers_component>()
                    .each([&](const component::owner_component& owner_component,
                              const component::skill_triggers_component& skill_triggers_component) {
                        if (owner_component.entity != strike_source_entity) {
                            return;
                        }
                        for (auto& skill_trigger : skill_triggers_component.skill_triggers) {
                            if (utils::conditions_satisfied(skill_trigger.condition,
                                                            strike_source_entity,
                                                            target_entity,
                                                            registry)) {
                                auto& strike_source_entity_skills_component =
                                    registry.get<component::skills_component>(strike_source_entity);
                                auto& skill_configuration =
                                    registry
                                        .get<component::skill_configuration_component>(
                                            strike_source_entity_skills_component.find_by(
                                                skill_trigger.skill))
                                        .skill_configuration;
                                utils::enqueue_child_skill(
                                    strike_source_entity, skill_configuration, registry);
                            }
                        }
                    });

                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(
                        strike_source_entity);
                std::copy(skill_configuration.on_strike_effect_applications.begin(),
                          skill_configuration.on_strike_effect_applications.end(),
                          std::back_inserter(outgoing_effects_component.effect_applications));
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
                int effective_duration =
                    utils::get_effective_effect_duration(application.base_duration_ms,
                                                         application.effect,
                                                         application_source_relative_attributes,
                                                         target_entity);
                if (!application.unique_effect.is_invalid()) {
                    utils::add_unique_effect_to_actor(application.unique_effect,
                                                      application.num_stacks,
                                                      effective_duration,
                                                      application_source_entity,
                                                      target_entity,
                                                      registry);
                }
                if (application.effect != actor::effect_t::INVALID) {
                    utils::add_effect_to_actor(application.effect,
                                               application.num_stacks,
                                               application.base_duration_ms,
                                               application_source_entity,
                                               target_entity,
                                               registry);
                }
            }
        });
}

}  // namespace gw2combat::system
