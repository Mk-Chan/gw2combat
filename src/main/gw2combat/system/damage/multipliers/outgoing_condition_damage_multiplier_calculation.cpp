#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>

#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
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
    ctx.registry.view<component::outgoing_damage_source>().each(
        [&](const entt::entity entity,
            const component::outgoing_damage_source& outgoing_damage_source) {
            auto source_entity = outgoing_damage_source.source;
            auto& source_outgoing_condition_damage_multiplier =
                ctx.registry.get<component::outgoing_condition_damage_multiplier>(source_entity);
            ctx.registry.emplace<component::outgoing_condition_damage_multiplier>(
                entity, source_outgoing_condition_damage_multiplier);
        });
}

}  // namespace gw2combat::system
