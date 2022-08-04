#ifndef GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP
#define GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/profession_components.hpp"

namespace gw2combat::system {

template <combat_stage stage>
static inline void virtue_of_justice(registry_t& registry, tick_t current_tick) {
    if constexpr (stage != combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION) {
        return;
    }

    registry.view<component::incoming_strike_damage>().each(
        [&](const component::incoming_strike_damage& incoming_strike_damage) {
            for (const auto& strike : incoming_strike_damage.strikes) {
                entity_t source_entity = utils::get_source_entity(strike.source, registry);
                auto virtue_of_justice_ptr =
                    registry.try_get<component::virtue_of_justice>(source_entity);
                if (virtue_of_justice_ptr) {
                    ++virtue_of_justice_ptr->num_unaccounted_hits;
                }
            }
        });

    registry.view<component::effective_attributes, component::virtue_of_justice>().each(
        [&](entity_t entity,
            const component::effective_attributes& effective_attributes,
            component::virtue_of_justice& virtue_of_justice) {
            unsigned int burning_stacks_to_apply =
                virtue_of_justice.num_unaccounted_hits /
                virtue_of_justice.number_of_ticks_for_burning_application;
            if (burning_stacks_to_apply > 0) {
                virtue_of_justice.num_unaccounted_hits %=
                    virtue_of_justice.number_of_ticks_for_burning_application;

                auto effective_duration = utils::get_effective_condition_duration(
                    2'000, effects::applied_effect_type::BURNING, effective_attributes);
                auto& outgoing_condition_application =
                    registry.get_or_emplace<component::outgoing_condition_application>(entity);
                outgoing_condition_application.effect_applications.template emplace_back(
                    effects::effect_application{effects::applied_effect_type::BURNING,
                                                burning_stacks_to_apply,
                                                effective_duration,
                                                entity});

                spdlog::info("tick: {}, entity: {}, outgoing burning from virtue_of_justice",
                             current_tick,
                             utils::get_entity_name(entity, registry));
            }
        });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_PROFESSION_PROFESSION_SYSTEMS_HPP
