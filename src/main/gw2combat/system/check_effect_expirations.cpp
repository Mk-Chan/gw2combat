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
void check_multi_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr) {
        for (effect& stack : effect_ptr->stacks) {
            if (ctx.current_tick >= stack.start_tick + stack.duration) {
                stack.is_expired = true;
            }
        }
    }
}

template <class Effect>
void check_single_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr &&
        ctx.current_tick >= effect_ptr->stack.start_tick + effect_ptr->stack.duration) {
        effect_ptr->stack.is_expired = true;
    }
}

void check_effect_expirations(context& ctx) {
    ctx.registry.view<component::is_character>().each([&](const entt::entity entity) {
        // Boons
        check_multi_stack_effect<component::might>(ctx, entity);
        check_single_stack_effect<component::fury>(ctx, entity);
        check_single_stack_effect<component::quickness>(ctx, entity);
        check_single_stack_effect<component::resolution>(ctx, entity);
        check_single_stack_effect<component::aegis>(ctx, entity);

        // Conditions
        check_multi_stack_effect<component::vulnerability>(ctx, entity);
        check_multi_stack_effect<component::burning>(ctx, entity);
    });
}

}  // namespace gw2combat::system
