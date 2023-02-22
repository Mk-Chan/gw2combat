#ifndef GW2COMBAT_COMBAT_LOOP_HPP
#define GW2COMBAT_COMBAT_LOOP_HPP

#include "common.hpp"

namespace gw2combat {

void local_combat_loop(const std::string& encounter_configuration_path);
std::string server_combat_loop(const std::string& encounter_configuration);

}  // namespace gw2combat

#endif  // GW2COMBAT_COMBAT_LOOP_HPP
