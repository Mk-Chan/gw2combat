#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/downstate.hpp"

namespace gw2combat::system {

void downstate_detection(registry_t& registry, tick_t current_tick) {
    registry.view<component::combat_stats>().each(
        [&](entity_t entity, const component::combat_stats& combat_stats) {
            if (combat_stats.health == 0) {
                registry.emplace_or_replace<component::downstate>(entity);
                // spdlog::info("entity: {}, now in downstate", static_cast<std::uint32_t>(entity));
            } else {
                registry.remove<component::downstate>(entity);
                // spdlog::info("entity: {}, not in downstate", static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system
