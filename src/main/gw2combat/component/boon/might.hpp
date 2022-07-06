#ifndef GW2COMBAT_COMPONENT_MIGHT_HPP
#define GW2COMBAT_COMPONENT_MIGHT_HPP

#include "gw2combat/effect.hpp"
#include "gw2combat/iterable_priority_queue.hpp"

namespace gw2combat::component {

struct might {
    iterable_priority_queue<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_MIGHT_HPP
