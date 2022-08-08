#ifndef GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP

#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/gear/sigils.hpp"
#include "gw2combat/system/system.hpp"

namespace gw2combat::system {

template <combat_stage stage>
void sigil_earth(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.template view<component::sigil_earth>().each([](component::sigil_earth& sigil_earth) {
        sigil_earth.cooldown_progress = std::min(sigil_earth.cooldown_progress + 1, tick_t{2'000});
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
                        spdlog::info("tick: {}, entity: {}, outgoing bleeding from earth sigil",
                                     current_tick,
                                     utils::get_entity_name(source_entity, registry));
                        strike.skill.on_hit_effect_applications.template emplace_back(
                            skills::effect_application{effects::effect_type::BLEEDING,
                                                       skills::applied_effect_direction::OUTGOING,
                                                       1,
                                                       6'000});
                        sigil_earth.cooldown_progress = 0;
                    }
                }
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_STAGED_GEAR_SYSTEMS_HPP
