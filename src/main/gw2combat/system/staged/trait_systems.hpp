#ifndef GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/effect_components.hpp"
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
                utils::has_trait(trait_type::SYMBOLIC_POWER, source_entity, registry);
            if (is_symbolic_power_traited) {
                for (strike& strike : outgoing_strike_damage.strikes) {
                    bool is_symbol_strike =
                        std::find(strike.skill.tags.begin(),
                                  strike.skill.tags.end(),
                                  skills::skill_tag::SYMBOL) != strike.skill.tags.end();
                    if (is_symbol_strike) {
                        strike.outgoing_strike_damage_multiplier *= 1.3;
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
                    utils::skill_has_tag(strike.skill, skills::skill_tag::SYMBOL);
                if (is_symbol_strike) {
                    auto source_entity = utils::get_source_entity(strike.source, registry);
                    if (utils::has_trait(trait_type::SYMBOLIC_AVENGER, source_entity, registry)) {
                        auto& symbolic_avenger_effect =
                            registry.get_or_emplace<component::symbolic_avenger_effect>(
                                source_entity);
                        symbolic_avenger_effect.effect_old.add(
                            effects::symbolic_avenger{source_entity, current_tick, 15'000});
                    }
                }
            }
        });
}

template <combat_stage stage>
void inspiring_virtue(registry_t& registry, tick_t current_tick) {
    if (stage != combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING) {
        return;
    }

    registry.template view<component::traits_component, component::instant_cast_skills>().each(
        [&](entity_t entity,
            const component::traits_component& traits_component,
            const component::instant_cast_skills& instant_cast_skills) {
            bool has_inspiring_virtue_trait =
                utils::has_trait(trait_type::INSPIRING_VIRTUE, traits_component);
            if (!has_inspiring_virtue_trait) {
                return;
            }

            for (const skills::skill& skill : instant_cast_skills.skills) {
                bool is_guardian_virtue_skill =
                    utils::skill_has_tag(skill, skills::skill_tag::GUARDIAN_VIRTUE);
                if (is_guardian_virtue_skill) {
                    registry.template emplace_or_replace<component::inspiring_virtue_effect>(
                        entity, effects::inspiring_virtue{entity, current_tick, 6'000});
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
