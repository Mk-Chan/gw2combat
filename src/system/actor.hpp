#ifndef GW2COMBAT_SYSTEM_ACTOR_HPP
#define GW2COMBAT_SYSTEM_ACTOR_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void setup_combat_stats(registry_t& registry);
extern void update_combat_stats(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ACTOR_HPP
