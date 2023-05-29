#ifndef GW2COMBAT_COMBAT_LOOP_HPP
#define GW2COMBAT_COMBAT_LOOP_HPP

#include "common.hpp"

#include "configuration/encounter.hpp"

namespace gw2combat {

extern void tick(registry_t& registry);

extern std::string combat_loop(const configuration::encounter_t& encounter_configuration,
                               bool enable_caching = false);

}  // namespace gw2combat

#endif  // GW2COMBAT_COMBAT_LOOP_HPP
