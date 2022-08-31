#ifndef GW2COMBAT_COMBAT_LOOP_HPP
#define GW2COMBAT_COMBAT_LOOP_HPP

#include "common.hpp"

namespace gw2combat {

extern void do_tick(registry_t& registry, tick_t current_tick);
extern void combat_loop();

}  // namespace gw2combat

#endif  // GW2COMBAT_COMBAT_LOOP_HPP
