#ifndef GW2COMBAT_COMPONENT_SKILLS_NORMAL_CAST_SKILL_HPP
#define GW2COMBAT_COMPONENT_SKILLS_NORMAL_CAST_SKILL_HPP

#include "gw2combat/skills.hpp"

namespace gw2combat::component {

struct normal_cast_skill {
    skills::skill skill;
    size_t next_hit_idx{0};
    size_t next_pulse_idx{0};
    tick_t progress{0};
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_NORMAL_CAST_SKILL_HPP
