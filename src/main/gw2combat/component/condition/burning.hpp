#ifndef GW2COMBAT_COMPONENT_BURNING_HPP
#define GW2COMBAT_COMPONENT_BURNING_HPP

#include "gw2combat/effect.hpp"

namespace gw2combat::component {

struct burning {
    burning() : stacks() {
    }
    burning(entt::entity source, size_t num_stacks, tick_t start_tick, tick_t duration) : stacks() {
        for (size_t i = 0; i < num_stacks; ++i) {
            stacks.emplace_back(effect{source, start_tick, duration, 0, false});
        }
    }

    std::vector<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_BURNING_HPP
