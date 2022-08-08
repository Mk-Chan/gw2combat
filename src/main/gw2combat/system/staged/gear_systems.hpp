#ifndef GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/gear/sigils.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat::system {

template <combat_stage stage>
void sigil_geomancy(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING) {
        return;
    }

    registry.template view<component::sigil_geomancy>().each([&](entity_t entity,
                                                                 component::sigil_geomancy& sigil) {
        sigil.cooldown_progress = std::min(sigil.cooldown_progress + 1, tick_t{9'000});

        if (utils::has_sigil_equipped(weapon_sigil::GEOMANCY, entity, registry) &&
            registry.template any_of<component::did_weapon_swap>(entity) &&
            sigil.cooldown_progress >= 9'000) {
            auto& effective_attributes =
                registry.template get<component::effective_attributes>(entity);
            auto& outgoing_condition_application =
                registry.template get_or_emplace<component::outgoing_condition_application>(entity);
            auto effective_duration = utils::get_effective_effect_duration(
                8'000, effects::effect_type::BLEEDING, effective_attributes);
            outgoing_condition_application.effect_applications.template emplace_back(
                effects::effect_application{
                    effects::effect_type::BLEEDING,
                    entity,
                    effective_duration,
                    1,
                });
            sigil.cooldown_progress = 0;

            spdlog::info("tick: {}, entity: {}, outgoing bleeding from geomancy sigil",
                         current_tick,
                         utils::get_entity_name(entity, registry));
        }
    });
}

template <combat_stage stage>
void sigil_earth(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.template view<component::sigil_earth>().each([](component::sigil_earth& sigil) {
        sigil.cooldown_progress = std::min(sigil.cooldown_progress + 1, tick_t{2'000});
    });

    registry.view<component::incoming_strike_damage>().each(
        [&](component::incoming_strike_damage& incoming_strike_damage) {
            for (strike& strike : incoming_strike_damage.strikes) {
                auto source_entity = utils::get_source_entity(strike.source, registry);
                auto& effective_attributes =
                    registry.template get<component::effective_attributes>(source_entity);
                if (utils::has_sigil_equipped(weapon_sigil::EARTH, source_entity, registry)) {
                    auto& sigil_earth =
                        registry.template get<component::sigil_earth>(source_entity);
                    // NOTE: Move the crit check to send whether it is a crit or not in the strike
                    //       object itself so the calculation is only at one place
                    if (sigil_earth.cooldown_progress >= 2'000 &&
                        utils::check_random_success(effective_attributes.critical_chance_pct)) {
                        strike.skill.on_hit_effect_applications.template emplace_back(
                            skills::effect_application{effects::effect_type::BLEEDING,
                                                       skills::applied_effect_direction::OUTGOING,
                                                       1,
                                                       6'000});
                        sigil_earth.cooldown_progress = 0;

                        spdlog::info("tick: {}, entity: {}, outgoing bleeding from earth sigil",
                                     current_tick,
                                     utils::get_entity_name(source_entity, registry));
                    }
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP
