#ifndef GW2COMBAT_COMPONENT_ACTOR_SKILLS_HPP
#define GW2COMBAT_COMPONENT_ACTOR_SKILLS_HPP

#include "common.hpp"

#include "actor/skill.hpp"

namespace gw2combat::component {

struct actor_skills {
    std::unordered_map<actor::skill_t, entity_t> skill_key_to_entity_map;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_SKILLS_HPP
