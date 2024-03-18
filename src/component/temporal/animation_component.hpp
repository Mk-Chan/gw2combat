#ifndef GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct animation_component {
    entity_t skill_entity = entt::tombstone;
    std::array<int, 2> duration{0, 0};
    std::array<int, 2> progress{0, 0};
};

struct already_performed_animation {};
struct animation_expired {};
struct is_afk {};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(animation_component,
                                                skill_entity,
                                                duration,
                                                progress)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ANIMATION_COMPONENT_HPP
