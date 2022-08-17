#ifndef GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP
#define GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP

#include "gw2combat/actor/weapon.hpp"

namespace gw2combat::component {

struct equipped_weapons {
    std::vector<actor::weapon> weapons;
};

struct current_weapon_set {
    actor::weapon_set set = actor::weapon_set::SET_1;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP
