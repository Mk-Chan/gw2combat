#ifndef GW2COMBAT_COMPONENT_BURNING_HPP
#define GW2COMBAT_COMPONENT_BURNING_HPP

#include "gw2combat/effect.hpp"
#include "gw2combat/iterable_priority_queue.hpp"

namespace gw2combat::component {

struct burning {
    double buffered_damage = 0.0;
    iterable_priority_queue<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_BURNING_HPP
