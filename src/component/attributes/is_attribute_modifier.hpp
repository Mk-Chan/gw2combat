#ifndef GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_MODIFIER_HPP
#define GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_MODIFIER_HPP

#include "configuration/attribute_modifier.hpp"

namespace gw2combat::component {

struct is_attribute_modifier {
    std::vector<configuration::attribute_modifier_t> attribute_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(is_attribute_modifier, attribute_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ATTRIBUTE_IS_ATTRIBUTE_MODIFIER_HPP
