#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <entt/entt.hpp>

#include "types.hpp"

namespace gw2combat {

struct effect {
    entt::entity source;
    tick_t start_tick;
    tick_t duration;
    tick_t last_ticked = 0;
    bool is_expired = false;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_EFFECT_HPP
