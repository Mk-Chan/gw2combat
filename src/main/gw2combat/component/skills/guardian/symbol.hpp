#ifndef GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SYMBOL_HPP
#define GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SYMBOL_HPP

#include <entt/entt.hpp>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct symbol {
    entt::entity source;
    tick_t start_tick;
    skills::skill skill;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_GUARDIAN_SYMBOL_HPP
