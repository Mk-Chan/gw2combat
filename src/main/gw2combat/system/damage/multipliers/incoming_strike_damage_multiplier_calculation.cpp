#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"
#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

void incoming_strike_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::effective_attributes, component::vulnerability>().each(
        [&](entity_t entity,
            const component::effective_attributes& effective_attributes,
            const component::vulnerability& vulnerability) {
            auto& incoming_strike_damage_multiplier =
                registry.emplace<component::incoming_strike_damage_multiplier>(entity);
            incoming_strike_damage_multiplier.multiplier =
                (1.0 + (double)vulnerability.effect.num_stacks() * 0.01) /
                effective_attributes.armor;
        });

    registry
        .view<component::is_actor>(
            entt::exclude<component::effective_attributes, component::vulnerability>)
        .each([&](entity_t entity) {
            registry.emplace<component::incoming_strike_damage_multiplier>(entity);
        });
}

}  // namespace gw2combat::system
