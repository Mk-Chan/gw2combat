#ifndef GW2COMBAT_SYSTEM_ACTOR_HPP
#define GW2COMBAT_SYSTEM_ACTOR_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void setup_combat_stats(registry_t& registry);
extern void reset_combat_stats(registry_t& registry);
extern void update_combat_stats(registry_t& registry);

extern void calculate_relative_attributes(registry_t& registry);

extern void perform_rotations(registry_t& registry);
extern void perform_skills(registry_t& registry);

extern void cleanup_finished_casting_skills(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ACTOR_HPP
