#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"

namespace gw2combat::system {

void update_health(registry_t& registry, tick_t current_tick) {
    // TODO: Change this to take incoming_damage_history instead which persists across N ticks
    //       so that we can defer updates to combat_stats and process them at a greater interval
    //       than once per tick. Also implement incoming healing and incoming_healing_history later.
    registry.view<component::effective_incoming_damage, component::combat_stats>().each(
        [&](entity_t entity,
            const component::effective_incoming_damage& effective_incoming_damage,
            component::combat_stats& combat_stats) {
            auto rounded_effective_incoming_damage =
                (unsigned int)std::round(effective_incoming_damage.value);
            if (rounded_effective_incoming_damage > combat_stats.health) {
                combat_stats.health = 0;
            } else {
                combat_stats.health -= rounded_effective_incoming_damage;
            }
            spdlog::info("tick: {}, entity: {}, health: {}",
                         current_tick,
                         utils::get_name(entity, registry),
                         combat_stats.health);
        });
}

}  // namespace gw2combat::system
