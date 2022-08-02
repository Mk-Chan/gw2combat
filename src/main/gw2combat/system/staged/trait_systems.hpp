#ifndef GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/effect_components.hpp"
#include "gw2combat/component/trait_components.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat::system {

template <combat_stage stage>
void symbolic_power(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING) {
        return;
    }

    registry.view<component::outgoing_strike_damage>().each(
        [&](entity_t entity, component::outgoing_strike_damage& outgoing_strike_damage) {
            auto source_entity = utils::get_source_entity(entity, registry);
            bool is_symbolic_power_traited =
                registry.template any_of<component::symbolic_power>(source_entity);
            if (is_symbolic_power_traited) {
                for (strike& strike : outgoing_strike_damage.strikes) {
                    bool is_symbol_strike =
                        std::find(strike.skill.tags.begin(),
                                  strike.skill.tags.end(),
                                  skills::skill_tag::SYMBOL) != strike.skill.tags.end();
                    if (is_symbol_strike) {
                        strike.outgoing_strike_damage_multiplier *=
                            1.0 + component::symbolic_power::symbol_strike_damage_increase;
                    }
                }
            }
        });
}

template <combat_stage stage>
void symbolic_avenger(registry_t& registry, tick_t current_tick) {
    if (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.view<component::incoming_strike_damage>().each(
        [&](const component::incoming_strike_damage& incoming_strike_damage) {
            for (const strike& strike : incoming_strike_damage.strikes) {
                bool is_symbol_strike =
                    std::find(strike.skill.tags.begin(),
                              strike.skill.tags.end(),
                              skills::skill_tag::SYMBOL) != strike.skill.tags.end();
                if (is_symbol_strike) {
                    auto source_entity = utils::get_source_entity(strike.source, registry);
                    if (registry.any_of<component::symbolic_avenger_trait>(source_entity)) {
                        auto& symbolic_avenger_effect =
                            registry.get_or_emplace<component::symbolic_avenger_effect>(
                                source_entity);
                        symbolic_avenger_effect.effect.add(
                            effects::symbolic_avenger{source_entity, current_tick, 15'000});
                    }
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
