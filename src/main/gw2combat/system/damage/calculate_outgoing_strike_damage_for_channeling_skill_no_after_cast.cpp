#include "gw2combat/system/system.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/channeling_skill.hpp"
#include "gw2combat/component/outgoing_strike_damage.hpp"
#include "gw2combat/component/outgoing_strike_damage_multiplier.hpp"

namespace gw2combat::system {

void calculate_outgoing_strike_damage_for_channeling_skill_no_after_cast(context& ctx) {
    ctx.registry
        .view<component::outgoing_strike_damage_multiplier,
              component::animation,
              component::channeling_skill>()
        .each([&](const entt::entity entity,
                  const component::outgoing_strike_damage_multiplier&
                      outgoing_strike_damage_multiplier,
                  const component::animation& animation,
                  component::channeling_skill& channeling_skill) {
            skills::skill skill = channeling_skill.skill;
            assert(skill.type == skills::skill::type::CHANNELING_NO_AFTER_CAST ||
                   skill.type == skills::skill::type::CHANNELING_WITH_AFTER_CAST);

            bool has_quickness = ctx.registry.any_of<component::quickness>(entity);
            auto skill_hit_rate =
                animation.required_ticks_for_completion[has_quickness] / skill.hits;
            bool skill_hits_this_tick =
                ctx.current_tick >= channeling_skill.last_hit_tick + skill_hit_rate;
            if (skill_hits_this_tick) {
                channeling_skill.last_hit_tick = ctx.current_tick;

                auto& outgoing_strike_damage =
                    ctx.registry.emplace<component::outgoing_strike_damage>(entity);
                outgoing_strike_damage.strikes.emplace_back(strike{
                    entity,
                    outgoing_strike_damage_multiplier.multiplier * skill.damage_coefficient});
            }
        });
}

}  // namespace gw2combat::system
