#include "gw2combat/system/system.hpp"

#include "gw2combat/component/combat_stats.hpp"
#include "gw2combat/component/incoming_strike_damage.hpp"
#include "gw2combat/component/outgoing_strike_damage.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

void incoming_strike_detection(context& ctx) {
    ctx.registry.view<component::outgoing_strike_damage, component::combat_stats>().each(
        [&](const entt::entity entity,
            const component::outgoing_strike_damage& outgoing_strike_damage,
            component::combat_stats& combat_stats) {
            auto targeting_ptr = ctx.registry.try_get<component::targeting>(entity);
            if (targeting_ptr) {
                auto& incoming_strikes =
                    ctx.registry.get_or_emplace<component::incoming_strike_damage>(
                        targeting_ptr->entity);
                for (strike strike : outgoing_strike_damage.strikes) {
                    incoming_strikes.strikes.push_back(strike);
                }

                combat_stats.num_hits += outgoing_strike_damage.strikes.size();
            }
        });
}

}  // namespace gw2combat::system
