#ifndef GW2COMBAT_COMPONENT_ACTOR_ANIMATION_HPP
#define GW2COMBAT_COMPONENT_ACTOR_ANIMATION_HPP

#include "actor/skill.hpp"

namespace gw2combat::component {

struct animation {
    actor::skill_t skill;
    std::array<int, 2> duration{0, 0};
    std::array<int, 2> progress{0, 0};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(animation, skill, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_ANIMATION_HPP
