#ifndef GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_MODIFIERS_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_MODIFIERS_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/modifiers/damage_modifiers.hpp"
#include "gw2combat/modifiers/attribute_modifiers.hpp"

namespace gw2combat::component {

struct incoming_damage_modifiers {
    std::unordered_map<entity_t, modifiers::damage_modifiers_t> source_entity_to_damage_modifiers;
    std::unordered_map<entity_t, modifiers::attribute_modifiers_t>
        source_entity_to_attribute_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_damage_modifiers,
                                                source_entity_to_damage_modifiers,
                                                source_entity_to_attribute_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_MODIFIERS_HPP
