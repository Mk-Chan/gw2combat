#ifndef GW2COMBAT_PULSE_CONDITION_HPP
#define GW2COMBAT_PULSE_CONDITION_HPP

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct pulse_condition {
    constexpr static inline tick_t pulse_rate = 1'000;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_PULSE_CONDITION_HPP
