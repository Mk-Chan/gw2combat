#include "system.hpp"

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/static_attributes.hpp"

namespace gw2combat::system {

void combat_detection(registry_t& registry, tick_t) {
    // NOTE: Currently just perpetually puts and keeps the character entities in combat unless they
    //       are already in combat
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::combat_stats>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            registry.emplace_or_replace<component::combat_stats>(
                entity, component::combat_stats{static_attributes.max_health});
        });
}

}  // namespace gw2combat::system
