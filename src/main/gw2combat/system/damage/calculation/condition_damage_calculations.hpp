#ifndef GW2COMBAT_SYSTEM_DAMAGE_CALCULATION_CONDITION_DAMAGE_CALCULATION_HPP
#define GW2COMBAT_SYSTEM_DAMAGE_CALCULATION_CONDITION_DAMAGE_CALCULATION_HPP

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/pulse_conditions.hpp"

#include "gw2combat/component/condition/burning.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"

namespace gw2combat::system {

extern double calculate_burning_damage(
    context& ctx,
    const effect& burning_effect,
    tick_t ticks_to_consider = component::pulse_conditions::pulse_rate);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_DAMAGE_CALCULATION_CONDITION_DAMAGE_CALCULATION_HPP
