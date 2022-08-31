#ifndef GW2COMBAT_COMPONENT_DURATION_HPP
#define GW2COMBAT_COMPONENT_DURATION_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct duration {
    int duration;
    int progress = 0;
};

struct duration_expired {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(duration, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DURATION_HPP
