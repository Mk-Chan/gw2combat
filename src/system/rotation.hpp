#ifndef GW2COMBAT_SYSTEM_ROTATION_HPP
#define GW2COMBAT_SYSTEM_ROTATION_HPP

#include "common.hpp"

namespace gw2combat::system {

// Returns whether rotation was performed by at least one actor.
extern bool perform_rotations(registry_t& registry);

extern void destroy_actors_with_no_rotation(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ROTATION_HPP
