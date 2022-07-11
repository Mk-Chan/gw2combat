#ifndef GW2COMBAT_COMPONENT_SUCCESSFUL_SKILL_CAST_HPP
#define GW2COMBAT_COMPONENT_SUCCESSFUL_SKILL_CAST_HPP

#include "gw2combat/skills.hpp"

namespace gw2combat::component {

struct successful_skill_cast {
    skills::skill skill;
    tick_t skill_cast_end;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SUCCESSFUL_SKILL_CAST_HPP
