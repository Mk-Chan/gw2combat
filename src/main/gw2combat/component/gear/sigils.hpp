#ifndef GW2COMBAT_COMPONENT_GEAR_SIGILS_HPP
#define GW2COMBAT_COMPONENT_GEAR_SIGILS_HPP

#include "gw2combat/types.hpp"

#include "gw2combat/weapon.hpp"

namespace gw2combat::component {

struct sigil_earth {
    tick_t cooldown_progress = 2'000;
};

}

#endif  // GW2COMBAT_COMPONENT_GEAR_SIGILS_HPP
