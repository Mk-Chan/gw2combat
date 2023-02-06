#ifndef GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_MODIFIERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_MODIFIERS_COMPONENT_HPP

#include "configuration/attribute_modifier.hpp"

namespace gw2combat::component {

struct attribute_modifiers_component {
    std::vector<configuration::attribute_modifier_t> attribute_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modifiers_component, attribute_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_ATTRIBUTE_MODIFIERS_COMPONENT_HPP
