#ifndef GW2COMBAT_COMPONENT_EFFECT_SOURCE_ACTOR_HPP
#define GW2COMBAT_COMPONENT_EFFECT_SOURCE_ACTOR_HPP

#include "common.hpp"

namespace gw2combat::component {

struct source_actor {
    entity_t entity;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(source_actor, entity)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECT_SOURCE_ACTOR_HPP
