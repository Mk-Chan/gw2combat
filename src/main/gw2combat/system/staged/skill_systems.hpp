#ifndef GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP

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

                auto outgoing_damage_source_entity =
                    utils::get_damage_source_entity(strike.source, registry);
                auto& effective_attributes = registry.template get<component::effective_attributes>(
                    outgoing_damage_source_entity);
                auto& outgoing_condition_application =
                    registry.template get_or_emplace<component::outgoing_condition_application>(
                        outgoing_damage_source_entity);
                for (const skills::effect_application& effect_application :
                     strike.skill.on_hit_effect_applications) {
                    // NOTE: After implementing incoming effect application, remove this and use the
                    //       appropriate queue for direction
                    assert(effect_application.effect_direction ==
                           skills::applied_effect_direction::OUTGOING);
                    if (effect_application.effect_type == skills::applied_effect_type::BURNING) {
                        outgoing_condition_application.append_burning_effects(
                            effects::burning{
                                outgoing_damage_source_entity,
                                current_tick,
                                tick_t{
                                    (unsigned int)(effect_application.duration *
                                                   (1.0 +
                                                    std::max(
                                                        effective_attributes.condition_duration_pct,
                                                        effective_attributes.burning_duration_pct) /
                                                        100.0))}},
                            effect_application.num_stacks);
                    } else if (effect_application.effect_type ==
                               skills::applied_effect_type::BINDING_BLADE) {
                        outgoing_condition_application.binding_blade_effect =
                            effects::binding_blade{outgoing_damage_source_entity,
                                                   current_tick,
                                                   effect_application.duration};
                    }
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_STAGED_SKILL_SYSTEMS_HPP
