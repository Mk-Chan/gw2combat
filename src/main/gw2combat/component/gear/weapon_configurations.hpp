#ifndef GW2COMBAT_COMPONENT_GEAR_WEAPON_CONFIGURATIONS_HPP
#define GW2COMBAT_COMPONENT_GEAR_WEAPON_CONFIGURATIONS_HPP

#include <vector>

#include "gw2combat/weapon.hpp"

namespace gw2combat::component {

struct available_weapon_configurations {
    std::vector<weapon_configuration> weapon_configurations;
};

struct equipped_weapon_set {
    weapon_set current_set;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(available_weapon_configurations, weapon_configurations)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(equipped_weapon_set, current_set)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_GEAR_WEAPON_CONFIGURATIONS_HPP
