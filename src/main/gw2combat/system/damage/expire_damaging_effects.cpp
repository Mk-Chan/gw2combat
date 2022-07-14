#include <spdlog/spdlog.h>

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/damage/buffered_condition_damage.hpp"

#include "gw2combat/component/skills/guardian/binding_blade.hpp"
#include "gw2combat/system/damage/calculation/condition_damage_calculations.hpp"

namespace gw2combat::system {

void expire_damaging_effects(context& ctx) {
    ctx.registry.view<component::burning>().each([&](const entt::entity entity,
                                                     component::burning& burning) {
        for (effect& effect : burning.stacks) {
            if (!effect.is_expired(ctx.current_tick)) {
                continue;
            }

            auto& buffered_condition_damage =
                ctx.registry.get_or_emplace<component::buffered_condition_damage>(entity);
            buffered_condition_damage.value +=
                calculate_burning_damage(ctx, effect, ctx.current_tick - effect.last_damaging_tick);
        }
        std::erase_if(burning.stacks,
                      [&](effect& effect) { return effect.is_expired(ctx.current_tick); });
        if (burning.stacks.empty()) {
            ctx.registry.remove<component::burning>(entity);
        }
    });
    // Special handling for binding blade since it bypasses all damage modifiers
    ctx.registry.view<component::binding_blade>().each(
        [&](const entt::entity entity, component::binding_blade& binding_blade) {
            effect& effect = binding_blade.stack;
            if (effect.is_expired(ctx.current_tick)) {
                auto& buffered_condition_damage =
                    ctx.registry.get_or_emplace<component::buffered_condition_damage>(entity);
                buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                    calculate_binding_blade_damage(
                        ctx, effect, ctx.current_tick - effect.last_damaging_tick);

                ctx.registry.remove<component::binding_blade>(entity);
            }
        });
}

}  // namespace gw2combat::system
