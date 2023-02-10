#ifndef GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP

#include "common.hpp"

namespace gw2combat::component {

struct buffered_condition_damage {
    double value = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(buffered_condition_damage, value)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
