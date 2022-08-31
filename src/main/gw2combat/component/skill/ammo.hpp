#ifndef GW2COMBAT_COMPONENT_SKILL_AMMO_HPP
#define GW2COMBAT_COMPONENT_SKILL_AMMO_HPP

namespace gw2combat::component {

struct ammo {
    int max_ammo;
    int current_ammo;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ammo, max_ammo, current_ammo)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_AMMO_HPP
