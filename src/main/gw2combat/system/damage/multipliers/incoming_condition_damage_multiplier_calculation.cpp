#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"

namespace gw2combat::system {

void incoming_condition_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::vulnerability>().each(
        [&](const entt::entity entity, const component::vulnerability& vulnerability) {
            auto& incoming_condition_damage =
                ctx.registry.emplace<component::incoming_condition_damage_multiplier>(entity);
            incoming_condition_damage.multiplier = 1.0 + (double)vulnerability.stacks.size() * 0.01;
        });

    ctx.registry.view<component::is_character>(entt::exclude<component::vulnerability>)
        .each([&](const entt::entity entity) {
            ctx.registry.emplace<component::incoming_condition_damage_multiplier>(entity);
        });
}

}  // namespace gw2combat::system
