#ifndef GW2COMBAT_COMPONENT_DAMAGE_PULSE_CONDITIONS_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_PULSE_CONDITIONS_HPP

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct pulse_conditions {
    constexpr static inline tick_t pulse_rate = 1'000;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_PULSE_CONDITIONS_HPP
