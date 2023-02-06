#ifndef GW2COMBAT_COMPONENT_TEMPORAL_RECHARGE_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_TEMPORAL_RECHARGE_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct recharge_component {
    int duration = 0;
    int progress = 0;
};

struct recharge_expired_component {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(recharge_component, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_TEMPORAL_RECHARGE_COMPONENT_HPP
