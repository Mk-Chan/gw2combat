#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <entt/entt.hpp>

#include "types.hpp"

namespace gw2combat {

struct effect {
    explicit effect(entt::entity source, tick_t start_tick, tick_t duration)
        : source(source),
          start_tick(start_tick),
          duration(duration),
          last_damaging_tick(start_tick) {
    }

    [[nodiscard]] inline bool is_expired(tick_t current_tick) const {
        return remaining(current_tick) <= 0.0;
    }
    [[nodiscard]] inline bool remaining(tick_t current_tick) const {
        return (start_tick + duration) - current_tick;
    }

    entt::entity source;
    tick_t start_tick;
    tick_t duration;
    tick_t last_damaging_tick;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_EFFECT_HPP
