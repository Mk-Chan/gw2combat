#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/combat_stats.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"

namespace gw2combat::system {

void update_health(context& ctx) {
    // TODO: Change this to take incoming_damage_history instead which persists across N ticks
    //       so that we can defer updates to combat_stats and process them at a greater interval
    //       than once per tick. Also implement incoming healing and incoming_healing_history later.
    ctx.registry.view<component::effective_incoming_damage, component::combat_stats>().each(
        [&](const entt::entity entity,
            const component::effective_incoming_damage& effective_incoming_damage,
            component::combat_stats& combat_stats) {
            if (effective_incoming_damage.value > combat_stats.health) {
                combat_stats.health = 0;
            } else {
                combat_stats.health -= effective_incoming_damage.value;
            }
            spdlog::info(
                "entity: {}, health: {}", static_cast<std::uint32_t>(entity), combat_stats.health);
        });
}

}  // namespace gw2combat::system
