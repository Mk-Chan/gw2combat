#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/downstate.hpp"

namespace gw2combat::system {

void downstate_detection(context& ctx) {
    ctx.registry.view<component::combat_stats>().each(
        [&](const entt::entity entity, const component::combat_stats& combat_stats) {
            if (combat_stats.health == 0) {
                ctx.registry.emplace_or_replace<component::downstate>(entity);
                // spdlog::info("entity: {}, now in downstate", static_cast<std::uint32_t>(entity));
            } else {
                ctx.registry.remove<component::downstate>(entity);
                // spdlog::info("entity: {}, not in downstate", static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system
