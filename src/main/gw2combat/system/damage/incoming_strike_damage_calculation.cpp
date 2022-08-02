#include "gw2combat/system/system.hpp"

#include <numeric>

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"

namespace gw2combat::system {

void incoming_strike_damage_calculation(registry_t& registry, tick_t current_tick) {
    registry.view<component::incoming_strike_damage_multiplier, component::incoming_strike_damage>()
        .each([&](entity_t entity,
                  const component::incoming_strike_damage_multiplier&
                      incoming_strike_damage_multiplier,
                  const component::incoming_strike_damage& incoming_damage) {
            double accumulated_incoming_damage = std::accumulate(
                incoming_damage.strikes.begin(),
                incoming_damage.strikes.end(),
                0.0,
                [&](const double accumulated, const strike& next) {
                    double this_strike_damage =
                        next.outgoing_strike_damage_multiplier * next.critical_hit_multiplier *
                        weapon_strength(next.skill.weapon_type) * next.skill.damage_coefficient *
                        incoming_strike_damage_multiplier.multiplier;
                    spdlog::info("tick: {}, entity: {}, incoming strike damage: {}, from skill: {}",
                                 current_tick,
                                 utils::get_name(entity, registry),
                                 this_strike_damage,
                                 next.skill.name);
                    return accumulated + this_strike_damage;
                });

            auto& effective_incoming_damage =
                registry.get_or_emplace<component::effective_incoming_damage>(entity);
            effective_incoming_damage.value += accumulated_incoming_damage;

            spdlog::info("tick: {}, entity: {}, total incoming strike damage: {}",
                         current_tick,
                         utils::get_name(entity, registry),
                         effective_incoming_damage.value);
        });
}

}  // namespace gw2combat::system
