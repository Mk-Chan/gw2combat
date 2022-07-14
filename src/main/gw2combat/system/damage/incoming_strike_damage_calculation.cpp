#include "gw2combat/system/system.hpp"

#include <numeric>

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"

namespace gw2combat::system {

void incoming_strike_damage_calculation(context& ctx) {
    ctx.registry
        .view<component::incoming_strike_damage_multiplier, component::incoming_strike_damage>()
        .each([&](const entt::entity entity,
                  const component::incoming_strike_damage_multiplier&
                      incoming_strike_damage_multiplier,
                  const component::incoming_strike_damage& incoming_damage) {
            double accumulated_incoming_damage =
                std::accumulate(incoming_damage.strikes.begin(),
                                incoming_damage.strikes.end(),
                                0.0,
                                [&](const double accumulated, const strike& next) {
                                    return accumulated + next.damage;
                                });

            auto effective_strike_damage =
                accumulated_incoming_damage * incoming_strike_damage_multiplier.multiplier;

            auto& effective_incoming_damage =
                ctx.registry.get_or_emplace<component::effective_incoming_damage>(entity);
            effective_incoming_damage.value += effective_strike_damage;

            spdlog::info("tick: {}, entity: {}, incoming strike damage: {}",
                         ctx.current_tick,
                         static_cast<std::uint32_t>(entity),
                         effective_strike_damage);
        });
}

}  // namespace gw2combat::system
