#ifndef GW2COMBAT_SYSTEM_ENCOUNTER_HPP
#define GW2COMBAT_SYSTEM_ENCOUNTER_HPP

#include "common.hpp"

#include "configuration/encounter.hpp"

namespace gw2combat::system {

extern void setup_encounter(registry_t& registry, const configuration::encounter_t& encounter);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_ENCOUNTER_HPP
