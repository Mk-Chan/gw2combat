#ifndef GW2COMBAT_COMPONENT_ACTOR_FINISHED_ANIMATION_HPP
#define GW2COMBAT_COMPONENT_ACTOR_FINISHED_ANIMATION_HPP

#include "actor/skill.hpp"

namespace gw2combat::component {

struct finished_animation {
    actor::skill_t skill;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(finished_animation, skill)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_FINISHED_ANIMATION_HPP
