#ifndef GW2COMBAT_COMPONENT_INSTANT_CAST_SKILLS_HPP
#define GW2COMBAT_COMPONENT_INSTANT_CAST_SKILLS_HPP

#include "gw2combat/skills.hpp"

namespace gw2combat::component {

struct instant_cast_skills {
    std::vector<skills::skill> skills;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_INSTANT_CAST_SKILLS_HPP
