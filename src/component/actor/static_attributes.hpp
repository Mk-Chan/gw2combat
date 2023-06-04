#ifndef GW2COMBAT_COMPONENT_ACTOR_STATIC_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_ACTOR_STATIC_ATTRIBUTES_HPP

#include "actor/attributes.hpp"

namespace gw2combat::component {

struct static_attributes {
    std::map<actor::attribute_t, double> attribute_value_map;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_STATIC_ATTRIBUTES_HPP
