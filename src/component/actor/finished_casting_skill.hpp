#ifndef GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP
#define GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP

#include "common.hpp"

namespace gw2combat::component {

struct finished_casting_skill {
    entity_t skill_entity = entt::tombstone;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(finished_casting_skill, skill_entity)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_FINISHED_CASTING_SKILL_HPP
