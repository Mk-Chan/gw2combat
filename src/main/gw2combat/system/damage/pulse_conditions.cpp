#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/pulse_conditions.hpp"
#include "gw2combat/component/skills/guardian/binding_blade.hpp"
#include "gw2combat/system/damage/calculation/condition_damage_calculations.hpp"

namespace gw2combat::system {

void pulse_conditions(context& ctx) {
    if (ctx.registry.any_of<component::pulse_conditions>(*singleton_entity)) {
        ctx.registry.view<component::burning>().each(
            [&](const entt::entity entity, component::burning& burning) {
                for (effect& effect : burning.stacks) {
                    auto& buffered_condition_damage =
                        ctx.registry.get_or_emplace<component::buffered_condition_damage>(entity);
                    buffered_condition_damage.value += calculate_burning_damage(
                        ctx, effect, ctx.current_tick - effect.last_damaging_tick);

                    effect.last_damaging_tick = ctx.current_tick;
                }
            });
        // Special handling for binding blade since it bypasses all damage modifiers
        ctx.registry.view<component::binding_blade>().each(
            [&](const entt::entity entity, component::binding_blade& binding_blade) {
                effect& effect = binding_blade.stack;
                auto& buffered_condition_damage =
                    ctx.registry.get_or_emplace<component::buffered_condition_damage>(entity);
                buffered_condition_damage.unaffected_by_incoming_multiplier_value +=
                    calculate_binding_blade_damage(
                        ctx, effect, ctx.current_tick - effect.last_damaging_tick);

                effect.last_damaging_tick = ctx.current_tick;
            });

        // Apply and reset any buffered condition damage
        ctx.registry
            .view<component::incoming_condition_damage_multiplier,
                  component::buffered_condition_damage>()
            .each([&](const entt::entity entity,
                      const component::incoming_condition_damage_multiplier&
                          incoming_condition_damage_multiplier,
                      component::buffered_condition_damage& buffered_condition_damage) {
                double effective_buffered_condition_damage =
                    buffered_condition_damage.value *
                        incoming_condition_damage_multiplier.multiplier +
                    buffered_condition_damage.unaffected_by_incoming_multiplier_value;

                auto& effective_incoming_damage =
                    ctx.registry.get_or_emplace<component::effective_incoming_damage>(
                        entity, component::effective_incoming_damage{0});
                effective_incoming_damage.value += effective_buffered_condition_damage;

                ctx.registry.remove<component::buffered_condition_damage>(entity);
                spdlog::info("tick: {}, entity: {}, incoming condition damage: {}",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity),
                             effective_buffered_condition_damage);
            });
    }
}

}  // namespace gw2combat::system
