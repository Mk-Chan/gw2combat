#ifndef GW2COMBAT_SYSTEM_HOOKS_HPP
#define GW2COMBAT_SYSTEM_HOOKS_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void on_strike_hooks(registry_t& registry);
extern void on_ammo_gained_hooks(registry_t& registry);
extern void on_begun_casting_skills_hooks(registry_t& registry);
extern void on_every_tick_hooks(registry_t& registry);

}  // namespace gw2combat::system

#endif // GW2COMBAT_SYSTEM_HOOKS_HPP
