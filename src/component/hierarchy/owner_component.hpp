#ifndef GW2COMBAT_COMPONENT_HIERARCHY_OWNER_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_HIERARCHY_OWNER_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct owner_component {
    entity_t entity;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(owner_component, entity)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_HIERARCHY_OWNER_COMPONENT_HPP
