#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

void incoming_condition_damage_multiplier_calculation(registry_t& registry, tick_t) {
    registry.view<component::vulnerability>().each(
        [&](entity_t entity, const component::vulnerability& vulnerability) {
            auto& incoming_condition_damage =
                registry.emplace<component::incoming_condition_damage_multiplier>(entity);
            incoming_condition_damage.multiplier =
                1.0 + (double)vulnerability.effect.num_stacks() * 0.01;
        });

    registry.view<component::is_actor>(entt::exclude<component::vulnerability>)
        .each([&](entity_t entity) {
            registry.emplace<component::incoming_condition_damage_multiplier>(entity);
        });
}

}  // namespace gw2combat::system
