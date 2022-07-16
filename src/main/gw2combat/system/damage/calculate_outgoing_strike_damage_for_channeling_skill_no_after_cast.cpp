#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/skills/channeling_skill.hpp"
#include "gw2combat/component/skills/guardian/symbol.hpp"
#include "gw2combat/component/traits/guardian/symbolic_avenger.hpp"
#include "gw2combat/component/traits/guardian/symbolic_power.hpp"

namespace gw2combat::system {

double symbol_damage = 0.0;

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
            assert(skill.type == skills::skill::type::CHANNELING_NO_AFTER_CAST);

            bool has_quickness = ctx.registry.any_of<component::quickness>(entity);
            tick_t skill_hit_rate = animation.skill.cast_duration[has_quickness] / skill.hits;
            bool skill_hits_this_tick =
                ctx.current_tick == 0 ||
                ctx.current_tick >= channeling_skill.last_hit_tick + skill_hit_rate;
            if (skill_hits_this_tick) {
                channeling_skill.last_hit_tick = ctx.current_tick;

                // NOTE: This block is a huge hack until I rework skills to have defined points at
                //       which they can have effects in the combat pipeline
                //       (like on_cast_start, during_cast, etc...)
                double additional_multiplier = 1.0;
                bool is_symbol = ctx.registry.any_of<component::symbol>(entity);
                if (is_symbol) {
                    auto& outgoing_damage_source =
                        ctx.registry.get<component::outgoing_damage_source>(entity);
                    auto symbolic_avenger_ptr = ctx.registry.try_get<component::symbolic_avenger>(
                        outgoing_damage_source.source);
                    symbolic_avenger_ptr->stacks.emplace_back(
                        outgoing_damage_source.source, ctx.current_tick, 15'000);

                    bool source_has_symbolic_power = ctx.registry.any_of<component::symbolic_power>(
                        outgoing_damage_source.source);
                    if (source_has_symbolic_power) {
                        additional_multiplier +=
                            component::symbolic_power::symbol_strike_damage_increase;
                    }
                    symbol_damage += outgoing_strike_damage_multiplier.multiplier *
                                     skill.damage_coefficient * additional_multiplier *
                                     skill.weapon.weapon_strength();
                }

                auto& outgoing_strike_damage =
                    ctx.registry.get_or_emplace<component::outgoing_strike_damage>(entity);
                outgoing_strike_damage.strikes.emplace_back(
                    strike{entity,
                           outgoing_strike_damage_multiplier.multiplier * skill.damage_coefficient *
                               additional_multiplier * skill.weapon.weapon_strength()});
            }
        });
}

}  // namespace gw2combat::system
