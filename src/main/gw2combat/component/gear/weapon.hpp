#ifndef GW2COMBAT_COMPONENT_GEAR_WEAPON_HPP
#define GW2COMBAT_COMPONENT_GEAR_WEAPON_HPP

#include <cstdint>

namespace gw2combat::component {

struct weapon {
    enum class type : std::uint32_t
    {
        SWORD,
        FOCUS,
        GREATSWORD,
    };

    type type;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_GEAR_WEAPON_HPP
