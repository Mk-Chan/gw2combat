#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <entt/entt.hpp>

#include "types.hpp"

namespace gw2combat {

struct effect {
    [[nodiscard]] bool is_expired(tick_t current_tick) const {
        return current_tick >= start_tick + duration;
    }

    entt::entity source;
    tick_t start_tick;
    tick_t duration;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_EFFECT_HPP
