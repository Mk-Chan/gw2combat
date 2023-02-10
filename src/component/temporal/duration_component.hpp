#ifndef GW2COMBAT_COMPONENT_TEMPORAL_DURATION_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_TEMPORAL_DURATION_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct duration_component {
    int duration = 0;
    int progress = 0;
};

struct duration_expired {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(duration_component, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_TEMPORAL_DURATION_COMPONENT_HPP
