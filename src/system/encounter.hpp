#ifndef GW2COMBAT_SYSTEM_ENCOUNTER_HPP
#define GW2COMBAT_SYSTEM_ENCOUNTER_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void setup_local_encounter(registry_t& registry, const std::string& encounter_configuration_path);
extern void setup_server_encounter(registry_t& registry, const std::string& encounter_configuration);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ENCOUNTER_HPP
