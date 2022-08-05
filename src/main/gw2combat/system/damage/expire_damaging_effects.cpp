#include <spdlog/spdlog.h>

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/buffered_condition_damage.hpp"

#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

template <typename EffectComponent>
void buffer_damage_for_end_of_life_effect(
    registry_t& registry,
    tick_t current_tick,
    entity_t entity,
    effects::effect_old<typename EffectComponent::effect_type> effect) {
    if (effect.is_expired(current_tick)) {
        auto& buffered_condition_damage =
            registry.get_or_emplace<component::buffered_condition_damage>(entity);

        if constexpr (std::is_same_v<decltype(effect), effects::binding_blade>) {
            buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                effect.damage_calculation(registry, current_tick);
        } else {
            buffered_condition_damage.value += effect.damage_calculation(registry, current_tick);
        }

        effect.update_last_damaging_tick(current_tick);
    }
}

template <typename EffectComponent>
void buffer_damage_and_expire_end_of_life_effects(registry_t& registry, tick_t current_tick) {
    registry.template view<EffectComponent>().each(
        [&](entity_t entity, EffectComponent& effect_component) {
            effect_component.effect_old.for_each(
                registry,
                current_tick,
                entity,
                buffer_damage_for_end_of_life_effect<typename EffectComponent::effect_type>);

            if constexpr (std::is_same_v<
                              decltype(effect_component.effect_old),
                              effects::stacking_effect<typename EffectComponent::effect_type>>) {
                effect_component.effect_old.remove_expired_effects(current_tick);
                if (effect_component.effect_old.num_stacks() == 0) {
                    registry.remove<EffectComponent>(entity);
                }
            } else if (effect_component.effect_old.is_expired(current_tick)) {
                registry.remove<EffectComponent>(entity);
            }
        });
}

void expire_damaging_effects(registry_t& registry, tick_t current_tick) {
    buffer_damage_and_expire_end_of_life_effects<component::burning>(registry, current_tick);
    buffer_damage_and_expire_end_of_life_effects<component::bleeding>(registry, current_tick);
    buffer_damage_and_expire_end_of_life_effects<component::binding_blade>(registry, current_tick);
}

}  // namespace gw2combat::system
