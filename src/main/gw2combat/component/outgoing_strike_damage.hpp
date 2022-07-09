#ifndef GW2COMBAT_COMPONENT_OUTGOING_STRIKE_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_OUTGOING_STRIKE_DAMAGE_HPP

#include <vector>

#include "gw2combat/strike.hpp"

namespace gw2combat::component {

struct outgoing_strike_damage {
    std::vector<strike> strikes;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_OUTGOING_STRIKE_DAMAGE_HPP
