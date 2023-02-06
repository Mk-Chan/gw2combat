#ifndef GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP
#define GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP

#include "actor/weapon.hpp"

namespace gw2combat::component {

struct weapon_t {
    actor::weapon_type type;
    actor::weapon_position position;
    actor::weapon_set set;
};

struct equipped_weapons {
    std::vector<weapon_t> weapons;
};

struct current_weapon_set {
    actor::weapon_set set = actor::weapon_set::SET_1;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EQUIPMENT_WEAPONS_HPP
