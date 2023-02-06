#ifndef GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP

#include "configuration/attribute_conversion.hpp"

namespace gw2combat::component {

struct attribute_conversions_component {
    std::vector<configuration::attribute_conversion_t> attribute_conversions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_conversions_component,
                                                attribute_conversions)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP
