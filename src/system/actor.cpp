#include "actor.hpp"

#include "component/actor/attribute_conversions_component.hpp"
#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/combat_stats.hpp"
#include "component/actor/effective_attributes.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
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

void calculate_effective_attributes(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::effective_attributes>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& effective_attributes = registry.emplace<component::effective_attributes>(entity);
            effective_attributes.attribute_value_map = static_attributes.attribute_value_map;
        });

    registry.view<component::owner_component, component::attribute_modifiers_component>().each(
        [&](entity_t entity,
            const component::owner_component& owner_actor,
            const component::attribute_modifiers_component& attribute_modifiers_component) {
            auto& effective_attributes =
                registry.get<component::effective_attributes>(owner_actor.entity);
            for (auto& attribute_modifier : attribute_modifiers_component.attribute_modifiers) {
                if (utils::conditions_satisfied(
                        attribute_modifier.condition, registry, owner_actor.entity)) {
                    effective_attributes.attribute_value_map[attribute_modifier.attribute] =
                        effective_attributes.attribute_value_map[attribute_modifier.attribute] *
                            attribute_modifier.multiplier +
                        attribute_modifier.addend;
                    spdlog::info("{} {} modifier {}",
                                 utils::get_entity_name(owner_actor.entity, registry),
                                 utils::get_entity_name(entity, registry),
                                 utils::to_string(attribute_modifier));
                }
            }
        });

    registry.view<component::owner_component, component::attribute_conversions_component>().each(
        [&](entity_t entity,
            const component::owner_component& owner_actor,
            const component::attribute_conversions_component& attribute_conversions_component) {
            auto& effective_attributes =
                registry.get<component::effective_attributes>(owner_actor.entity);
            std::unordered_map<actor::attribute_t, double> attribute_conversion_bonuses;
            for (auto& attribute_conversion :
                 attribute_conversions_component.attribute_conversions) {
                if (utils::conditions_satisfied(
                        attribute_conversion.condition, registry, owner_actor.entity)) {
                    attribute_conversion_bonuses[attribute_conversion.to] +=
                        (int)(attribute_conversion_bonuses[attribute_conversion.from] *
                                  attribute_conversion.multiplier +
                              attribute_conversion.addend);
                    spdlog::info("{} {} conversion {}",
                                 utils::get_entity_name(owner_actor.entity, registry),
                                 utils::get_entity_name(entity, registry),
                                 utils::to_string(attribute_conversion));
                }
            }
            for (auto&& [attribute, bonus] : attribute_conversion_bonuses) {
                effective_attributes.attribute_value_map[attribute] += bonus;
            }
        });

    registry.view<component::effective_attributes, component::effects_component>().each(
        [&](component::effective_attributes& effective_attributes,
            const component::effects_component& effects_component) {
            if (effects_component.has(actor::effect_t::FURY)) {
                effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER] +=
                    25.0;
            }

            int might_stacks = effects_component.count(actor::effect_t::MIGHT);
            effective_attributes.attribute_value_map[actor::attribute_t::POWER] +=
                30.0 * (double)might_stacks;
            effective_attributes.attribute_value_map[actor::attribute_t::CONDITION_DAMAGE] +=
                30.0 * (double)might_stacks;
        });
}

}  // namespace gw2combat::system
