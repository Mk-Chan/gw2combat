#ifndef GW2COMBAT_COMPONENT_OWNER_ACTOR_HPP
#define GW2COMBAT_COMPONENT_OWNER_ACTOR_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct owner_actor {
    entity_t entity;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(owner_actor, entity)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_OWNER_ACTOR_HPP
