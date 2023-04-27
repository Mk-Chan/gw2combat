#ifndef GW2COMBAT_SYSTEM_ROTATION_HPP
#define GW2COMBAT_SYSTEM_ROTATION_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void perform_rotations(registry_t& registry);
extern void perform_skills(registry_t& skill_trigger_entity);

extern void cleanup_casting_skills(registry_t& registry);
extern void destroy_actors_with_no_rotation(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ROTATION_HPP
