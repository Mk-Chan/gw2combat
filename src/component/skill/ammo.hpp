#ifndef GW2COMBAT_COMPONENT_SKILL_AMMO_HPP
#define GW2COMBAT_COMPONENT_SKILL_AMMO_HPP

#include "common.hpp"

namespace gw2combat::component {

struct ammo {
    int max_ammo = 1;
    int current_ammo = 1;
};

struct ammo_gained {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ammo, max_ammo, current_ammo)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_AMMO_HPP
