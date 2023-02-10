#ifndef GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct animation_component {
    std::array<int, 2> duration{0, 0};
    std::array<int, 2> progress{0, 0};
};

struct animation_expired {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(animation_component, duration, progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP
