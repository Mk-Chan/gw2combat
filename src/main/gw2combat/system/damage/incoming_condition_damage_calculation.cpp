#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"

namespace gw2combat::system {

void expire_effects(registry_t& registry, tick_t current_tick) {
    registry.view<component::effects_component>().each(
        [&](entity_t entity, component::effects_component& effects_component) {
            auto& buffered_condition_damage =
                registry.get_or_emplace<component::buffered_condition_damage_old>(entity);

            for (auto&& [effect_type, effect_stack] : effects_component.effects) {
                if (effect_stack.progress >= effect_stack.duration) {
                    if (utils::is_effect_affected_by_incoming_multiplier(effect_type)) {
                        buffered_condition_damage.value +=
                            utils::calculate_condition_damage(effect_type, effect_stack, registry);
                    } else {
                        buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                            utils::calculate_condition_damage(effect_type, effect_stack, registry);
                    }
                }
            }
            std::erase_if(effects_component.effects, [&](const auto& item) {
                const auto& [effect_type, effect_stack] = item;
                return effect_stack.progress >= effect_stack.duration;
            });
            if (buffered_condition_damage.value == 0.0 &&
                buffered_condition_damage.unaffected_by_incoming_multiplier_value == 0.0) {
                registry.remove<component::buffered_condition_damage_old>(entity);
            }
        });
}

void progress_effects(registry_t& registry, tick_t current_tick) {
    registry.view<component::effects_component>().each(
        [&](component::effects_component& effects_component) {
            for (auto&& [effect_type, effect_stacks] : effects_component.effects) {
                effect_stacks.progress += 1;
            }
        });
}

void incoming_condition_damage_calculation(registry_t& registry, tick_t current_tick) {
    registry.view<component::effects_component>().each(
        [&](entity_t entity, component::effects_component& effects_component) {
            auto& buffered_condition_damage =
                registry.get_or_emplace<component::buffered_condition_damage_old>(entity);

            for (auto&& [effect_type, effect_stack] : effects_component.effects) {
                if (utils::is_effect_affected_by_incoming_multiplier(effect_type)) {
                    buffered_condition_damage.value +=
                        utils::calculate_condition_damage(effect_type, effect_stack, registry);
                } else {
                    buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                        utils::calculate_condition_damage(effect_type, effect_stack, registry);
                }

                effect_stack.duration -= effect_stack.progress;
                effect_stack.progress = 0;
            }

            if (buffered_condition_damage.value == 0.0 &&
                buffered_condition_damage.unaffected_by_incoming_multiplier_value == 0.0) {
                registry.remove<component::buffered_condition_damage_old>(entity);
            }
        });

    registry
        .view<component::incoming_condition_damage_multiplier,
              component::buffered_condition_damage_old>()
        .each([&](entity_t entity,
                  const component::incoming_condition_damage_multiplier&
                      incoming_condition_damage_multiplier,
                  component::buffered_condition_damage_old& buffered_condition_damage) {
            double effective_buffered_condition_damage =
                buffered_condition_damage.value * incoming_condition_damage_multiplier.multiplier +
                buffered_condition_damage.unaffected_by_incoming_multiplier_value;

            auto& effective_incoming_damage =
                registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += effective_buffered_condition_damage;

            registry.remove<component::buffered_condition_damage_old>(entity);
            spdlog::info("tick: {}, entity: {}, incoming condition damage: {}",
                         current_tick,
                         utils::get_entity_name(entity, registry),
                         effective_buffered_condition_damage);
        });
}

}  // namespace gw2combat::system
