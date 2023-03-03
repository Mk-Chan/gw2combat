#ifndef GW2COMBAT_COMPONENT_EFFECT_SOURCE_SKILL_HPP
#define GW2COMBAT_COMPONENT_EFFECT_SOURCE_SKILL_HPP

#include "common.hpp"

#include "actor/skill.hpp"

namespace gw2combat::component {

struct source_skill {
    actor::skill_t skill;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(source_skill, skill)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECT_SOURCE_SKILL_HPP
