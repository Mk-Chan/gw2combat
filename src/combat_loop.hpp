#ifndef GW2COMBAT_COMBAT_LOOP_HPP
#define GW2COMBAT_COMBAT_LOOP_HPP

#include "common.hpp"

namespace gw2combat {

extern void tick(registry_t& registry);

extern void local_combat_loop(const std::string& encounter_configuration_path);
extern std::string server_combat_loop(const std::string& encounter_configuration);

}  // namespace gw2combat

#endif  // GW2COMBAT_COMBAT_LOOP_HPP
