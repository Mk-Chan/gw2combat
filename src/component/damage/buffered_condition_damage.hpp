#ifndef GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP

#include "common.hpp"

namespace gw2combat::component {

struct buffered_condition_damage {
    std::map<std::tuple<entity_t, actor::effect_t>, double> source_entity_effect_tuple_to_value_map;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(buffered_condition_damage,
                                                source_entity_effect_tuple_to_value_map)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
