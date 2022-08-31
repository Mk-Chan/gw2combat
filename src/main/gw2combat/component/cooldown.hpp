#ifndef GW2COMBAT_COMPONENT_COOLDOWN_HPP
#define GW2COMBAT_COMPONENT_COOLDOWN_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct cooldown {
    std::array<int, 2> duration;
    std::array<int, 2> progress = {0, 0};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(cooldown, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COOLDOWN_HPP
