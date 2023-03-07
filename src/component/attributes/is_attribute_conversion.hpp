#ifndef GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_CONVERSION_HPP
#define GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_CONVERSION_HPP

#include "configuration/attribute_conversion.hpp"

namespace gw2combat::component {

struct is_attribute_conversion {
    std::vector<configuration::attribute_conversion_t> attribute_conversions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(is_attribute_conversion, attribute_conversions)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_CONVERSION_HPP
