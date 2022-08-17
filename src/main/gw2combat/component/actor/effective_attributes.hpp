#ifndef GW2COMBAT_COMPONENT_ACTOR_EFFECTIVE_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_ACTOR_EFFECTIVE_ATTRIBUTES_HPP

#include "gw2combat/actor/attributes.hpp"

namespace gw2combat::component {

struct effective_attributes {
    inline double operator[](actor::attribute_t attribute) const {
        return attribute_value_map.find(attribute)->second;
    }

    std::unordered_map<actor::attribute_t, double> attribute_value_map;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effective_attributes, attribute_value_map)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_EFFECTIVE_ATTRIBUTES_HPP
