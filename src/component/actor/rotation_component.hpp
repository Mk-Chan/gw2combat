#ifndef GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP

#include "actor/rotation.hpp"

namespace gw2combat::component {

struct rotation_component {
    actor::rotation_t rotation;
    std::size_t current_idx = 0;
    tick_t offset = 0;
    bool repeat = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(rotation_component, rotation, current_idx, repeat)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP
