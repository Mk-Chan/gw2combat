#ifndef GW2COMBAT_COMPONENT_CHANNELING_SKILL_HPP
#define GW2COMBAT_COMPONENT_CHANNELING_SKILL_HPP

#include "gw2combat/skills.hpp"

namespace gw2combat::component {

struct channeling_skill {
    skills::skill skill;
    tick_t last_hit_tick;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_CHANNELING_SKILL_HPP
