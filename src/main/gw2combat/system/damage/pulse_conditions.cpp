#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/effect_components.hpp"

#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/pulse_conditions.hpp"

namespace gw2combat::system {

template <typename EffectComponent, bool is_affected_by_incoming_multiplier = true>
void buffer_effect_damage(registry_t& registry, tick_t current_tick) {
    registry.view<EffectComponent>().each([&](entity_t entity, EffectComponent& effect_component) {
        auto& buffered_condition_damage =
            registry.get_or_emplace<component::buffered_condition_damage>(entity);

        if constexpr (is_affected_by_incoming_multiplier) {
            buffered_condition_damage.value +=
                effect_component.effect.damage_calculation(registry, current_tick);
        } else {
            buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                effect_component.effect.damage_calculation(registry, current_tick);
        }

        effect_component.effect.update_last_damaging_tick(current_tick);
    });
}

void pulse_conditions(registry_t& registry, tick_t current_tick) {
    if (!registry.ctx().contains<component::pulse_conditions>()) {
        return;
    }

    buffer_effect_damage<component::burning>(registry, current_tick);
    buffer_effect_damage<component::binding_blade, false>(registry, current_tick);

    // Apply and reset any buffered condition damage
    registry
        .view<component::incoming_condition_damage_multiplier,
              component::buffered_condition_damage>()
        .each([&](entity_t entity,
                  const component::incoming_condition_damage_multiplier&
                      incoming_condition_damage_multiplier,
                  component::buffered_condition_damage& buffered_condition_damage) {
            double effective_buffered_condition_damage =
                buffered_condition_damage.value * incoming_condition_damage_multiplier.multiplier +
                buffered_condition_damage.unaffected_by_incoming_multiplier_value;

            auto& effective_incoming_damage =
                registry.get_or_emplace<component::effective_incoming_damage>(
                    entity, component::effective_incoming_damage{0});
            effective_incoming_damage.value += effective_buffered_condition_damage;

            registry.remove<component::buffered_condition_damage>(entity);
            spdlog::info("tick: {}, entity: {}, incoming condition damage: {}",
                         current_tick,
                         utils::get_name(entity, registry),
                         effective_buffered_condition_damage);
        });
}

}  // namespace gw2combat::system
