#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"

namespace gw2combat::system {

template <class Effect>
void expire_multi_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr) {
        std::vector<effect>& stacks = effect_ptr->stacks;
        std::erase_if(stacks, [&](effect& effect) { return effect.is_expired(ctx.current_tick); });
        if (stacks.empty()) {
            ctx.registry.template remove<Effect>(entity);
        }
    }
}

template <class Effect>
void expire_single_stack_effect(context& ctx, entt::entity entity) {
    Effect* effect_ptr = ctx.registry.template try_get<Effect>(entity);
    if (effect_ptr && effect_ptr->stack.is_expired(ctx.current_tick)) {
        ctx.registry.template remove<Effect>(entity);
    }
}

void expire_non_damaging_effects(context& ctx) {
    ctx.registry.view<component::is_character>().each([&](const entt::entity entity) {
        // Boons
        expire_multi_stack_effect<component::might>(ctx, entity);
        expire_single_stack_effect<component::fury>(ctx, entity);
        expire_single_stack_effect<component::quickness>(ctx, entity);
        expire_single_stack_effect<component::resolution>(ctx, entity);
        expire_single_stack_effect<component::aegis>(ctx, entity);

        // Conditions
        expire_multi_stack_effect<component::vulnerability>(ctx, entity);
    });
}

}  // namespace gw2combat::system
