#ifndef GW2COMBAT_COMPONENT_SKILL_CASTING_SKILL_HPP
#define GW2COMBAT_COMPONENT_SKILL_CASTING_SKILL_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct casting_skill {
    size_t next_strike_idx = 0;
    size_t next_pulse_idx = 0;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_CASTING_SKILL_HPP
