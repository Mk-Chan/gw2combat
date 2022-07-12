#include <spdlog/spdlog.h>

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/damage/buffered_condition_damage.hpp"

#include "gw2combat/system/damage/calculation/condition_damage_calculations.hpp"

namespace gw2combat::system {

void expire_damaging_effects(context& ctx) {
    ctx.registry.view<component::burning>().each(
        [&](const entt::entity entity, component::burning& burning) {
            for (const effect& effect : burning.stacks) {
                if (!effect.is_expired(ctx.current_tick)) {
                    continue;
                }

                auto& buffered_condition_damage =
                    ctx.registry.get_or_emplace<component::buffered_condition_damage>(entity);
                tick_t ticks_since_last_condition_damage_pulse =
                    ctx.current_tick % component::pulse_conditions::pulse_rate;
                buffered_condition_damage.value +=
                    calculate_burning_damage(ctx, burning, ticks_since_last_condition_damage_pulse);
            }
            std::erase_if(burning.stacks,
                          [&](effect& effect) { return effect.is_expired(ctx.current_tick); });
        });
}

}  // namespace gw2combat::system
