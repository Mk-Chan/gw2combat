#ifndef GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SWORD_OF_JUSTICE_HPP
#define GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SWORD_OF_JUSTICE_HPP

#include <entt/entt.hpp>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct sword_of_justice {
    constexpr static inline tick_t damage_start_after_spawn = 650;
    constexpr static inline tick_t duration = 2'250;

    entt::entity source;
    tick_t start_tick;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SWORD_OF_JUSTICE_HPP
