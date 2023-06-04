#ifndef GW2COMBAT_COMPONENT_ACTOR_RELATIVE_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_ACTOR_RELATIVE_ATTRIBUTES_HPP

#include "actor/attributes.hpp"

namespace gw2combat::component {

struct relative_attributes {
    [[nodiscard]] inline double get(entity_t target_entity, actor::attribute_t attribute) const {
        return entity_and_attribute_to_value_map.at(target_entity).at(attribute);
    }
    inline double set(entity_t target_entity, actor::attribute_t attribute, double value) {
        return entity_and_attribute_to_value_map[target_entity][attribute] = value;
    }

    std::map<entity_t, std::map<actor::attribute_t, double>> entity_and_attribute_to_value_map;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(relative_attributes,
                                                entity_and_attribute_to_value_map)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_RELATIVE_ATTRIBUTES_HPP
