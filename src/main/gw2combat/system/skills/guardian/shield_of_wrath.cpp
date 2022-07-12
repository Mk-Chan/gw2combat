#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/skills.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/skills/guardian/shield_of_wrath.hpp"

namespace gw2combat::system {

void shield_of_wrath(context& ctx) {
    ctx.registry.view<component::outgoing_strike_damage_multiplier, component::shield_of_wrath>()
        .each([&](const entt::entity entity,
                  const component::outgoing_strike_damage_multiplier&
                      outgoing_strike_damage_multiplier,
                  const component::shield_of_wrath& shield_of_wrath) {
            if (ctx.current_tick >= shield_of_wrath.end_tick) {
                double damage_coefficient = skills::GUARDIAN_FOCUS_5.damage_coefficient;
                auto& outgoing_strike_damage =
                    ctx.registry.get_or_emplace<component::outgoing_strike_damage>(entity);
                outgoing_strike_damage.strikes.emplace_back(strike{
                    entity, outgoing_strike_damage_multiplier.multiplier * damage_coefficient});
                ctx.registry.remove<component::shield_of_wrath>(entity);

                spdlog::info("tick: {}, entity: {}, shield of wrath exploded",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system
