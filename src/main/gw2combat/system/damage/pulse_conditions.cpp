#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/pulse_conditions.hpp"
#include "gw2combat/system/damage/calculation/condition_damage_calculations.hpp"

namespace gw2combat::system {

void pulse_conditions(context& ctx) {
    if (ctx.current_tick % component::pulse_conditions::pulse_rate == 0) {
        ctx.registry.emplace<component::pulse_conditions>(*singleton_entity);
        spdlog::info("tick: {}, pulsing condition damage", ctx.current_tick);

        ctx.registry.view<component::incoming_condition_damage_multiplier, component::burning>()
            .each([&](const entt::entity entity,
                      const component::incoming_condition_damage_multiplier&
                          incoming_condition_damage_multiplier,
                      const component::burning& burning) {
                double effective_burning_damage = calculate_burning_damage(ctx, burning) *
                                                  incoming_condition_damage_multiplier.multiplier;

                auto& effective_incoming_damage =
                    ctx.registry.get_or_emplace<component::effective_incoming_damage>(entity);
                effective_incoming_damage.value += effective_burning_damage;

                spdlog::info(
                    "entity: {}, stacks: {}, incoming condition damage: {}, effective incoming "
                    "damage: "
                    "{}",
                    static_cast<std::uint32_t>(entity),
                    burning.stacks.size(),
                    effective_burning_damage,
                    effective_incoming_damage.value);
            });

        // Apply and reset any buffered condition damage from partial applications
        ctx.registry
            .view<component::incoming_condition_damage_multiplier,
                  component::buffered_condition_damage>()
            .each([&](const entt::entity entity,
                      const component::incoming_condition_damage_multiplier&
                          incoming_condition_damage_multiplier,
                      component::buffered_condition_damage& buffered_condition_damage) {
                double effective_buffered_condition_damage =
                    buffered_condition_damage.value *
                    incoming_condition_damage_multiplier.multiplier;

                auto& effective_incoming_damage =
                    ctx.registry.get_or_emplace<component::effective_incoming_damage>(
                        entity, component::effective_incoming_damage{0});
                effective_incoming_damage.value += effective_buffered_condition_damage;

                ctx.registry.remove<component::buffered_condition_damage>(entity);
                spdlog::info("entity: {}, taking buffered condition damage: {}",
                             static_cast<std::uint32_t>(entity),
                             effective_buffered_condition_damage);
            });
    }
}

}  // namespace gw2combat::system
