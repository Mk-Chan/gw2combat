#ifndef GW2COMBAT_COMPONENT_VIRTUE_OF_COURAGE_HPP
#define GW2COMBAT_COMPONENT_VIRTUE_OF_COURAGE_HPP

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct virtue_of_justice {
    unsigned int consecutive_successful_hits;
    tick_t apply_burning_on_tick;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_VIRTUE_OF_COURAGE_HPP
