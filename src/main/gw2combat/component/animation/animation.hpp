#ifndef GW2COMBAT_COMPONENT_ANIMATION_HPP
#define GW2COMBAT_COMPONENT_ANIMATION_HPP

#include <cstdint>

#include "gw2combat/types.hpp"

using namespace entt::literals;

namespace gw2combat::component {

struct animation {
    [[nodiscard]] constexpr inline tick_t end_tick(bool has_quickness) const {
        if (!skill.hits) {
            return start_tick + skill.cast_duration[has_quickness];
        }
        return start_tick + (skill.cast_duration[has_quickness] / skill.hits) * skill.hits;
    }

    skills::skill skill;
    tick_t start_tick;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ANIMATION_HPP
