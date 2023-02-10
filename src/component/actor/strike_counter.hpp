#ifndef GW2COMBAT_COMPONENT_ACTOR_STRIKE_COUNTER_HPP
#define GW2COMBAT_COMPONENT_ACTOR_STRIKE_COUNTER_HPP

#include "common.hpp"

namespace gw2combat::component {

struct strike_counter {
    int value = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_counter, value)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_STRIKE_COUNTER_HPP
