#ifndef GW2COMBAT_COMPONENT_ACTOR_BEGUN_CASTING_SKILLS_HPP
#define GW2COMBAT_COMPONENT_ACTOR_BEGUN_CASTING_SKILLS_HPP

#include "common.hpp"

namespace gw2combat::component {

struct begun_casting_skills {
    std::vector<entity_t> skill_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(begun_casting_skills, skill_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_BEGUN_CASTING_SKILLS_HPP
