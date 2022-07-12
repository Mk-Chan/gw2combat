#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>

#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/gear/sigil/sigil_bursting.hpp"

namespace gw2combat::system {

void outgoing_condition_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::sigil_bursting>().each([&](const entt::entity entity) {
        auto& outgoing_condition_damage_multiplier =
            ctx.registry.emplace<component::outgoing_condition_damage_multiplier>(entity);
        outgoing_condition_damage_multiplier.multiplier =
            1.0 + component::sigil_bursting::outgoing_condition_damage_increase;
    });
    ctx.registry.view<component::is_character>(entt::exclude<component::sigil_bursting>)
        .each([&](const entt::entity entity) {
            ctx.registry.emplace<component::outgoing_condition_damage_multiplier>(entity);
        });
}

}  // namespace gw2combat::system
