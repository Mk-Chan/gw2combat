#ifndef GW2COMBAT_CONFIGURATION_WEAPON_HPP
#define GW2COMBAT_CONFIGURATION_WEAPON_HPP

#include "actor/weapon.hpp"

namespace gw2combat::configuration {

struct weapon {
    actor::weapon_type type;
    actor::weapon_position position;
    actor::weapon_set set;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(weapon, type, position, set)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_WEAPON_HPP
