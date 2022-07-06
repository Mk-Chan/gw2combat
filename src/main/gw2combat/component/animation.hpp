#ifndef GW2COMBAT_COMPONENT_ANIMATION_HPP
#define GW2COMBAT_COMPONENT_ANIMATION_HPP

#include <cstdint>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct animation {
    // TODO: make a better implementation for animation later
    enum state : std::uint32_t
    {
        IDLE,
        CAST_SKILL_GUARDIAN_GREATSWORD_1_1,
        CAST_SKILL_GUARDIAN_GREATSWORD_1_2,
        CAST_SKILL_GUARDIAN_GREATSWORD_1_3,
    };

    state current_state;
    tick_t start_tick;
    tick_t accumulated_ticks;
    tick_t required_ticks_for_completion;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ANIMATION_HPP
