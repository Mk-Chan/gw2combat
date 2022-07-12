#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"

namespace gw2combat::system {

void incoming_strike_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::effective_attributes, component::vulnerability>().each(
        [&](const entt::entity entity,
            const component::effective_attributes& effective_attributes,
            const component::vulnerability& vulnerability) {
            auto& incoming_strike_damage_multiplier =
                ctx.registry.emplace<component::incoming_strike_damage_multiplier>(entity);
            incoming_strike_damage_multiplier.multiplier =
                (1.0 + (double)vulnerability.stacks.size() * 0.01) / effective_attributes.armor;
        });

    ctx.registry
        .view<component::is_character>(
            entt::exclude<component::effective_attributes, component::vulnerability>)
        .each([&](const entt::entity entity) {
            ctx.registry.emplace<component::incoming_strike_damage_multiplier>(entity);
        });
}

}  // namespace gw2combat::system
