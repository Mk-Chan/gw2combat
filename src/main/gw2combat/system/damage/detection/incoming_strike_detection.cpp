#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/damage/source_entity.hpp"

namespace gw2combat::system {

void incoming_strike_detection(registry_t& registry, tick_t) {
    registry
        .view<component::source_entity, component::outgoing_strike_damage>(
            entt::exclude<component::targeting>)
        .each([&](const component::source_entity& outgoing_damage_source,
                  const component::outgoing_strike_damage& outgoing_strike_damage) {
            auto targeting_ptr =
                registry.try_get<component::targeting>(outgoing_damage_source.entity);
            if (targeting_ptr) {
                auto& incoming_strikes = registry.get_or_emplace<component::incoming_strike_damage>(
                    targeting_ptr->entity);
                for (const strike& strike : outgoing_strike_damage.strikes) {
                    incoming_strikes.strikes.push_back(strike);
                }
            }
        });
    registry.view<component::targeting, component::outgoing_strike_damage>().each(
        [&](const component::targeting& targeting,
            const component::outgoing_strike_damage& outgoing_strike_damage) {
            auto& incoming_strikes =
                registry.get_or_emplace<component::incoming_strike_damage>(targeting.entity);
            for (const strike& strike : outgoing_strike_damage.strikes) {
                incoming_strikes.strikes.push_back(strike);
            }
        });
}

}  // namespace gw2combat::system
