#include "system.hpp"

#include "gw2combat/component/character/combat_stats.hpp"
#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/character/static_attributes.hpp"

namespace gw2combat::system {

void combat_detection(context& ctx) {
    ctx.registry
        .view<component::is_character, component::static_attributes, component::dynamic_attributes>(
            entt::exclude<component::combat_stats>)
        .each([&](const entt::entity entity,
                  const component::static_attributes& static_attributes,
                  const component::dynamic_attributes& dynamic_attributes) {
            // NOTE: Currently just perpetually puts and keeps the character entities in combat
            ctx.registry.emplace_or_replace<component::combat_stats>(
                entity,
                component::combat_stats{static_attributes.max_health,
                                        dynamic_attributes.max_endurance});
        });
}

}  // namespace gw2combat::system
