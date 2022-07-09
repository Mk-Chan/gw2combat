#ifndef GW2COMBAT_COMPONENT_MIGHT_HPP
#define GW2COMBAT_COMPONENT_MIGHT_HPP

#include "gw2combat/effect.hpp"
#include "gw2combat/iterable_priority_queue.hpp"

namespace gw2combat::component {

struct might {
    might() : stacks() {
    }
    might(entt::entity source, size_t num_stacks, tick_t end_tick) : stacks() {
        for (size_t i = 0; i < num_stacks; ++i) {
            stacks.emplace(effect{source, end_tick});
        }
    }

    iterable_priority_queue<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_MIGHT_HPP
