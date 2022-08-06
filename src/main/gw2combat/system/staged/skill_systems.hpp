#ifndef GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_condition_application.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat::system {

template <combat_stage stage>
void on_hit_effect_applications(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.template view<component::incoming_strike_damage>().each(
        [&](const component::incoming_strike_damage& incoming_strike_damage) {
            for (const strike& strike : incoming_strike_damage.strikes) {
                if (strike.skill.on_hit_effect_applications.empty()) {
                    continue;
                }

                auto source_entity = utils::get_source_entity(strike.source, registry);
                auto& effective_attributes =
                    registry.template get<component::effective_attributes>(source_entity);
                auto& outgoing_condition_application =
                    registry.template get_or_emplace<component::outgoing_condition_application>(
                        source_entity);
                auto& incoming_condition_application =
                    registry.template get_or_emplace<component::incoming_condition_application>(
                        source_entity);
                for (const skills::effect_application& effect_application :
                     strike.skill.on_hit_effect_applications) {
                    auto effective_duration =
                        utils::get_effective_effect_duration(effect_application.duration,
                                                             effect_application.effect_type,
                                                             effective_attributes);
                    if (effect_application.effect_direction ==
                        skills::applied_effect_direction::OUTGOING) {
                        outgoing_condition_application.effect_applications.template emplace_back(
                            effects::effect_application{
                                effect_application.effect_type,
                                source_entity,
                                effective_duration,
                                effect_application.num_stacks,
                            });
                    } else if (effect_application.effect_direction ==
                               skills::applied_effect_direction::INCOMING) {
                        incoming_condition_application.effect_applications.template emplace_back(
                            effects::effect_application{effect_application.effect_type,
                                                        source_entity,
                                                        effective_duration,
                                                        effect_application.num_stacks});
                    }
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP
