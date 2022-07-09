#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"
#include "gw2combat/component/is_character.hpp"

namespace gw2combat::system {

template <class Effect>
void relax_multi_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr) {
        while (!effect_ptr->stacks.empty() &&
               ctx.current_tick > effect_ptr->stacks.top().end_tick) {
            effect_ptr->stacks.pop();
        }
        if (effect_ptr->stacks.empty()) {
            ctx.registry.remove<Effect>(entity);
        }
    }
}

template <class Effect>
void relax_single_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr && ctx.current_tick > effect_ptr->stack.end_tick) {
        ctx.registry.remove<Effect>(entity);
    }
}

void effect_expiration(context& ctx) {
    ctx.registry.view<component::is_character>().each([&](const entt::entity entity) {
        // Boons
        relax_multi_stack_effect<component::might>(ctx, entity);
        relax_single_stack_effect<component::fury>(ctx, entity);
        relax_single_stack_effect<component::quickness>(ctx, entity);
        relax_single_stack_effect<component::resolution>(ctx, entity);
        relax_single_stack_effect<component::aegis>(ctx, entity);

        // Conditions
        relax_multi_stack_effect<component::vulnerability>(ctx, entity);
        relax_multi_stack_effect<component::burning>(ctx, entity);
    });
}

}  // namespace gw2combat::system
