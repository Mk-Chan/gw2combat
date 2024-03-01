#ifndef GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP

#include "actor/rotation.hpp"

namespace gw2combat::component {

struct rotation_component {
    actor::rotation_t rotation;
    int current_idx = 0;
    tick_t tick_offset = 0;
    bool repeat = false;

    std::list<actor::skill_cast_t> queued_rotation{};
};

struct already_performed_rotation {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(rotation_component,
                                                rotation,
                                                current_idx,
                                                tick_offset,
                                                repeat,
                                                queued_rotation)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_ROTATION_COMPONENT_HPP
