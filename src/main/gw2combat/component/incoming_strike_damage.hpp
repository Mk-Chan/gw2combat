#ifndef GW2COMBAT_COMPONENT_INCOMING_STRIKE_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_INCOMING_STRIKE_DAMAGE_HPP

#include "gw2combat/strike.hpp"

namespace gw2combat::component {

struct incoming_strike_damage {
    std::vector<strike> strikes;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_INCOMING_STRIKE_DAMAGE_HPP
