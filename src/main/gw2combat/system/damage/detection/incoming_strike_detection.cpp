#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"

namespace gw2combat::system {

void incoming_strike_detection(context& ctx) {
    ctx.registry.view<component::outgoing_damage_source, component::outgoing_strike_damage>().each(
        [&](const component::outgoing_damage_source& outgoing_damage_source,
            const component::outgoing_strike_damage& outgoing_strike_damage) {
            auto targeting_ptr =
                ctx.registry.try_get<component::targeting>(outgoing_damage_source.source);
            if (targeting_ptr) {
                auto& incoming_strikes =
                    ctx.registry.get_or_emplace<component::incoming_strike_damage>(
                        targeting_ptr->entity);
                for (strike strike : outgoing_strike_damage.strikes) {
                    incoming_strikes.strikes.push_back(strike);
                }
            }
        });
    ctx.registry.view<component::targeting, component::outgoing_strike_damage>().each(
        [&](const component::targeting& targeting,
            const component::outgoing_strike_damage& outgoing_strike_damage) {
            auto& incoming_strikes =
                ctx.registry.get_or_emplace<component::incoming_strike_damage>(targeting.entity);
            for (strike strike : outgoing_strike_damage.strikes) {
                incoming_strikes.strikes.push_back(strike);
            }
        });
}

}  // namespace gw2combat::system
