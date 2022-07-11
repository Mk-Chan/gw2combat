#ifndef GW2COMBAT_COMPONENT_ANIMATION_HPP
#define GW2COMBAT_COMPONENT_ANIMATION_HPP

#include <cstdint>

#include "gw2combat/types.hpp"

using namespace entt::literals;

namespace gw2combat::component {

struct animation {
    constexpr static inline entt::hashed_string IDLE = "idle"_hs;

    [[nodiscard]] constexpr inline tick_t end_tick(bool has_quickness) const {
        return start_tick + required_ticks_for_completion[has_quickness];
    }

    entt::hashed_string name;
    tick_t start_tick;
    std::array<tick_t, 2> required_ticks_for_completion;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ANIMATION_HPP
