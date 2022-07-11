#include "gw2combat/system/system.hpp"

#include <entt/entt.hpp>

#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/gear/sigil/sigil_bursting.hpp"
#include "gw2combat/component/outgoing_condition_damage_multiplier.hpp"

namespace gw2combat::system {

void outgoing_condition_damage_multiplier_calculation(context& ctx) {
    ctx.registry.view<component::effective_attributes>().each(
        [&](const entt::entity entity,
            const component::effective_attributes& effective_attributes) {
            bool has_bursting_sigil = ctx.registry.any_of<component::sigil_bursting>(entity);
            double bursting_sigil_multiplier =
                1.0 +
                has_bursting_sigil * component::sigil_bursting::outgoing_condition_damage_increase;

            ctx.registry.emplace<component::outgoing_condition_damage_multiplier>(
                entity, component::outgoing_condition_damage_multiplier{bursting_sigil_multiplier});
        });
}

}  // namespace gw2combat::system
