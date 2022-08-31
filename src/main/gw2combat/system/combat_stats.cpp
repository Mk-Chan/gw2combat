#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/component/actor/combat_stats.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/is_downstate.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/damage/incoming_damage.hpp"
#include "gw2combat/component/owner_actor.hpp"
#include "gw2combat/utilities/logging_utilities.hpp"

namespace gw2combat::system {

void init_combat_stats(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_actor, component::combat_stats>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            registry.emplace<component::combat_stats>(
                entity,
                component::combat_stats{
                    (int)static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH)});
        });
}

void update_combat_stats(registry_t& registry) {
    bool health_updated = false;
    registry
        .view<component::combat_stats, component::incoming_damage>(
            entt::exclude<component::is_downstate>)
        .each([&](entity_t entity,
                  component::combat_stats& combat_stats,
                  const component::incoming_damage& incoming_damage) {
            combat_stats.health -= (int)incoming_damage.value;
            health_updated = true;

            if (combat_stats.health <= 0) {
                registry.emplace<component::is_downstate>(entity);
            }
        });
    if (health_updated) {
        utils::log_component<component::combat_stats>(registry);
    }
}

}  // namespace gw2combat::system
