#include <spdlog/spdlog.h>

#include "gw2combat/skills.hpp"

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/skills/successful_skill_cast.hpp"

namespace gw2combat::system {

void calculate_outgoing_strike_damage_for_casting_skill_no_after_cast(context& ctx) {
    ctx.registry
        .view<component::outgoing_strike_damage_multiplier, component::successful_skill_cast>()
        .each([&](const entt::entity entity,
                  const component::outgoing_strike_damage_multiplier&
                      outgoing_strike_damage_multiplier,
                  const component::successful_skill_cast& successful_skill_cast) {
            skills::skill skill = successful_skill_cast.skill;
            if (skill.type == skills::skill::type::CHANNELING_NO_AFTER_CAST) {
                return;
            }

            auto& outgoing_strike_damage =
                ctx.registry.get_or_emplace<component::outgoing_strike_damage>(entity);
            outgoing_strike_damage.strikes.emplace_back(strike{
                entity, outgoing_strike_damage_multiplier.multiplier * skill.damage_coefficient});
        });
}

}  // namespace gw2combat::system
