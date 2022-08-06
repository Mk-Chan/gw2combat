#ifndef GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat::system {

template <combat_stage stage>
void unrelenting_criticism(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.view<component::incoming_strike_damage>().each(
        [&](component::incoming_strike_damage& incoming_strike_damage) {
            for (strike& strike : incoming_strike_damage.strikes) {
                auto source_entity = utils::get_source_entity(strike.source, registry);
                if (utils::has_trait(trait_type::UNRELENTING_CRITICISM, source_entity, registry) &&
                    strike.skill.weapon_type == weapon_type::AXE &&
                    utils::check_random_success(33.0)) {
                    spdlog::info(
                        "tick: {}, entity: {}, outgoing bleeding from unrelenting criticism",
                        current_tick,
                        utils::get_entity_name(source_entity, registry));
                    strike.skill.on_hit_effect_applications.template emplace_back(
                        skills::effect_application{effects::effect_type::BLEEDING,
                                                   skills::applied_effect_direction::OUTGOING,
                                                   1,
                                                   3'000});
                }
            }
        });
}

template <combat_stage stage>
void symbolic_power(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING) {
        return;
    }

    registry.view<component::outgoing_strike_damage>().each(
        [&](entity_t entity, component::outgoing_strike_damage& outgoing_strike_damage) {
            auto source_entity = utils::get_source_entity(entity, registry);
            if (utils::has_trait(trait_type::SYMBOLIC_POWER, source_entity, registry)) {
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
                if (utils::skill_has_tag(strike.skill, skills::skill_tag::SYMBOL)) {
                    auto source_entity = utils::get_source_entity(strike.source, registry);
                    if (utils::has_trait(trait_type::SYMBOLIC_AVENGER, source_entity, registry)) {
                        auto& effects_component =
                            registry.template get<component::effects_component>(source_entity);
                        utils::apply_effects(
                            effects::effect_application{
                                effects::effect_type::SYMBOLIC_AVENGER, source_entity, 15'000},
                            effects_component);
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
                    auto& effects_component =
                        registry.template get<component::effects_component>(entity);
                    utils::apply_effects(
                        effects::effect_application{
                            effects::effect_type::INSPIRING_VIRTUE, entity, 6'000},
                        effects_component);
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_TRAITS_TRAIT_SYSTEMS_HPP
