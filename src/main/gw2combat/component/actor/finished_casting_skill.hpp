#ifndef GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP
#define GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP

#include "gw2combat/actor/skill.hpp"

namespace gw2combat::component {

struct finished_casting_skill {
    actor::skill_t skill;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(finished_casting_skill, skill)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP
