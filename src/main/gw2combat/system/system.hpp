#ifndef GW2COMBAT_SYSTEM_HPP
#define GW2COMBAT_SYSTEM_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::system {

extern void progress_recharges(registry_t& registry);
extern void progress_cooldowns(registry_t& registry);
extern void calculate_effective_attributes(registry_t& registry);
extern void continue_rotation(registry_t& registry);
extern void do_animation(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_HPP
