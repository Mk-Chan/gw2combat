#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <entt/entt.hpp>

#include "types.hpp"

namespace gw2combat {

struct effect {
    entt::entity source;
    tick_t end_tick;

    static inline bool compare_by_remaining_duration(effect& left, effect& right) {
        return left.end_tick > right.end_tick;
    }
};

}  // namespace gw2combat

#endif  // GW2COMBAT_EFFECT_HPP
