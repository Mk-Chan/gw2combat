#include "actor.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/is_downstate.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/counter/is_counter.hpp"
#include "component/counter/is_counter_modifier.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/hierarchy/owner_component.hpp"

#include "utils/condition_utils.hpp"
#include "utils/counter_utils.hpp"
#include "utils/io_utils.hpp"

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

void update_counters(registry_t& registry) {
    registry.view<component::is_counter_modifier_t>().each(
        [&](entity_t counter_modifier_entity,
            const component::is_counter_modifier_t& is_counter_modifier) {
            auto owner_actor = utils::get_owner(counter_modifier_entity, registry);
            for (auto& counter_modifier : is_counter_modifier.counter_modifiers) {
                auto& counter = utils::get_counter(counter_modifier.counter_key, registry);
                bool independent_conditions_satisfied = utils::independent_conditions_satisfied(
                    counter_modifier.condition, owner_actor, std::nullopt, registry);
                if (independent_conditions_satisfied) {
                    utils::apply_counter_modifications(registry, counter, counter_modifier);
                }
            }
        });
}

}  // namespace gw2combat::system
