#include "actor.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/counter/is_counter.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/hierarchy/owner_component.hpp"

#include "utils/condition_utils.hpp"

namespace gw2combat::system {

void setup_combat_stats(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::combat_stats>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            registry.emplace<component::combat_stats>(
                entity,
                component::combat_stats{utils::round_to_nearest_even(
                    static_attributes.attribute_value_map.at(actor::attribute_t::MAX_HEALTH))});
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
            double total_incoming_damage =
                std::accumulate(incoming_damage.incoming_damage_events.begin(),
                                incoming_damage.incoming_damage_events.end(),
                                0.0,
                                [](double accumulated,
                                   const component::incoming_damage_event& incoming_damage_event) {
                                    return accumulated + incoming_damage_event.value;
                                });
            combat_stats.health -= utils::round_to_nearest_even(total_incoming_damage);
            health_updated = true;

            if (combat_stats.health <= 0) {
                registry.emplace<component::is_downstate>(entity);
            }
            registry.emplace_or_replace<component::combat_stats_updated>(entity);
        });
    // if (health_updated) {
    //     utils::log_component<component::combat_stats>(registry);
    // }
}

void reset_counters(registry_t& registry) {
    registry.view<component::is_counter>().each([&](component::is_counter& is_counter) {
        int reset_at_value = is_counter.counter_configuration.reset_at_value;
        int current_value = is_counter.value;
        if (current_value >= reset_at_value) {
            is_counter.value = current_value - reset_at_value;
        }
    });
}

}  // namespace gw2combat::system
