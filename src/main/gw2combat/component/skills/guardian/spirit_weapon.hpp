#ifndef GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SPIRIT_WEAPON_HPP
#define GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SPIRIT_WEAPON_HPP

#include <entt/entt.hpp>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct spirit_weapon {
    entt::entity source;
    tick_t start_tick;
    skills::skill skill;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SPIRIT_WEAPON_HPP
