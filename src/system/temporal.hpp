#ifndef GW2COMBAT_SYSTEM_TEMPORAL_HPP
#define GW2COMBAT_SYSTEM_TEMPORAL_HPP

#include "common.hpp"

namespace gw2combat::system {

// Returns whether at least one actor made any animation progress.
extern bool progress_animations(registry_t& registry);
extern void progress_cooldowns(registry_t& registry);
extern void progress_durations(registry_t& registry);
extern void progress_casting_skill_ticks(registry_t& registry);
extern void progress_casting_skills(registry_t& registry);
extern void cleanup_expired_components(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_TEMPORAL_HPP
