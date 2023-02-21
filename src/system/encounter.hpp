#ifndef GW2COMBAT_SYSTEM_ENCOUNTER_HPP
#define GW2COMBAT_SYSTEM_ENCOUNTER_HPP

#include "common.hpp"

namespace gw2combat::system {

void setup_encounter(registry_t& registry, const std::string& encounter_configuration_path);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ENCOUNTER_HPP
