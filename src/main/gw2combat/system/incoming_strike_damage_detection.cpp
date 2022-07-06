#include "system.hpp"

#include "gw2combat/component/incoming_damage.hpp"
#include "gw2combat/component/outgoing_damage.hpp"
#include "gw2combat/component/targeting.hpp"

namespace gw2combat::system {

void incoming_strike_damage_detection(context& ctx) {
    ctx.registry.view<component::outgoing_damage, component::targeting>().each(
        [&](const entt::entity entity,
            const component::outgoing_damage& outgoing_damage,
            const component::targeting& targeting) {
            auto& incoming_damage =
                ctx.registry.get_or_emplace<component::incoming_damage>(targeting.entity);
            incoming_damage.damage_data_vec.emplace_back(
                component::incoming_damage::damage_data{entity, outgoing_damage.value});
        });
}

}  // namespace gw2combat::system
