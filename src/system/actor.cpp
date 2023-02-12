#include "actor.hpp"

#include "component/actor/attribute_conversions_component.hpp"
#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/hierarchy/owner_component.hpp"

#include "utils/condition_utils.hpp"
#include "utils/io_utils.hpp"

namespace gw2combat::system {

void setup_combat_stats(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::combat_stats>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            registry.emplace<component::combat_stats>(
                entity,
                component::combat_stats{
                    (int)static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH)});
        });
}

void reset_combat_stats(registry_t& registry) {
    registry.view<component::static_attributes, component::combat_stats>().each(
        [&](const component::static_attributes& static_attributes,
            component::combat_stats& combat_stats) {
            combat_stats.health =
                (int)static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH);
        });
}

void update_combat_stats(registry_t& registry) {
    bool health_updated = false;
    registry
        .view<component::combat_stats, component::incoming_damage>(
            entt::exclude<component::is_downstate>)
        .each([&](entity_t entity,
                  component::combat_stats& combat_stats,
                  const component::incoming_damage& incoming_damage) {
            combat_stats.health -= (int)incoming_damage.value;
            health_updated = true;

            if (combat_stats.health <= 0) {
                registry.emplace<component::is_downstate>(entity);
            }
        });
    if (health_updated) {
        utils::log_component<component::combat_stats>(registry);
    }
}

void calculate_relative_attributes(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::relative_attributes>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& relative_attributes = registry.emplace<component::relative_attributes>(entity);
            registry
                .view<component::is_actor, component::static_attributes>(
                    entt::exclude<component::owner_component>)
                .each([&](entity_t other_entity, const component::static_attributes&) {
                    if (entity == other_entity) {
                        return;
                    }
                    for (auto& [attribute, value] : static_attributes.attribute_value_map) {
                        relative_attributes.set(other_entity, attribute, value);
                    }
                });
        });

    registry.view<component::owner_component, component::attribute_modifiers_component>().each(
        [&](entity_t,
            const component::owner_component& owner_actor,
            const component::attribute_modifiers_component& attribute_modifiers_component) {
            auto source_entity = owner_actor.entity;
            auto& relative_attributes = registry.get<component::relative_attributes>(source_entity);
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_entity, const component::relative_attributes&) {
                    if (source_entity == other_entity) {
                        return;
                    }
                    for (auto& attribute_modifier :
                         attribute_modifiers_component.attribute_modifiers) {
                        if (utils::conditions_satisfied(attribute_modifier.condition,
                                                        registry,
                                                        source_entity,
                                                        other_entity)) {
                            relative_attributes.set(
                                other_entity,
                                attribute_modifier.attribute,
                                relative_attributes.get(other_entity,
                                                        attribute_modifier.attribute) *
                                        attribute_modifier.multiplier +
                                    attribute_modifier.addend);
                            // spdlog::info("{} {} modifier {} against {}",
                            //              utils::get_entity_name(source_entity, registry),
                            //              utils::get_entity_name(entity, registry),
                            //              utils::to_string(attribute_modifier),
                            //              utils::get_entity_name(other_entity, registry));
                        }
                    }
                });
        });

    registry.view<component::owner_component, component::attribute_conversions_component>().each(
        [&](entity_t,
            const component::owner_component& owner_actor,
            const component::attribute_conversions_component& attribute_conversions_component) {
            auto source_entity = owner_actor.entity;
            auto& relative_attributes =
                registry.get<component::relative_attributes>(source_entity);
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_entity, const component::relative_attributes&) {
                    if (source_entity == other_entity) {
                        return;
                    }

                    std::unordered_map<actor::attribute_t, double> attribute_conversion_bonuses;
                    for (auto& attribute_conversion :
                         attribute_conversions_component.attribute_conversions) {
                        if (utils::conditions_satisfied(attribute_conversion.condition,
                                                        registry,
                                                        source_entity,
                                                        other_entity)) {
                            attribute_conversion_bonuses[attribute_conversion.to] +=
                                (int)(attribute_conversion_bonuses[attribute_conversion.from] *
                                          attribute_conversion.multiplier +
                                      attribute_conversion.addend);
                        }
                    }

                    for (auto&& [attribute, bonus] : attribute_conversion_bonuses) {
                        relative_attributes.set(
                            other_entity,
                            attribute,
                            relative_attributes.get(other_entity, attribute) + bonus);
                        // spdlog::info(
                        //     "{} {} conversion {} against {}",
                        //     utils::get_entity_name(source_entity, registry),
                        //     utils::get_entity_name(entity, registry),
                        //     utils::to_string(relative_attributes.get(other_entity, attribute)),
                        //     utils::get_entity_name(other_entity, registry));
                    }
                });
        });
}

}  // namespace gw2combat::system
