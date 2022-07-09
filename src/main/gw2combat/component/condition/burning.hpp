#ifndef GW2COMBAT_COMPONENT_BURNING_HPP
#define GW2COMBAT_COMPONENT_BURNING_HPP

#include "gw2combat/effect.hpp"
#include "gw2combat/iterable_priority_queue.hpp"

namespace gw2combat::component {

struct burning {
    burning() : buffered_damage(0.0), stacks() {
    }
    burning(entt::entity source, size_t num_stacks, tick_t end_tick)
        : buffered_damage(0.0), stacks() {
        for (size_t i = 0; i < num_stacks; ++i) {
            stacks.emplace(effect{source, end_tick});
        }
    }

    double buffered_damage;
    iterable_priority_queue<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_BURNING_HPP
