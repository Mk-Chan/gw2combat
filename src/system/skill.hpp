#ifndef GW2COMBAT_SYSTEM_SKILL_HPP
#define GW2COMBAT_SYSTEM_SKILL_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void perform_skill_ticks(registry_t& registry);
extern void perform_skills(registry_t& registry);

extern void cleanup_skill_ticks_tracker(registry_t& registry);
extern void cleanup_skill_actions(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_SKILL_HPP
