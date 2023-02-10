#ifndef GW2COMBAT_COMPONENT_COOLDOWN_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_COOLDOWN_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct cooldown_component {
    std::array<int, 2> duration;
    std::array<int, 2> progress = {0, 0};
};

struct cooldown_expired {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(cooldown_component, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COOLDOWN_COMPONENT_HPP
