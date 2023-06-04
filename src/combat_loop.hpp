#ifndef GW2COMBAT_COMBAT_LOOP_HPP
#define GW2COMBAT_COMBAT_LOOP_HPP

#include "common.hpp"

#include "audit/report.hpp"
#include "configuration/encounter.hpp"

namespace gw2combat {

extern void tick(registry_t& registry);

extern audit::report_t combat_loop(const configuration::encounter_t& encounter_configuration,
                                   bool enable_caching = false);

extern void search_rotation_for_encounter(
    const configuration::encounter_t& encounter_configuration);

}  // namespace gw2combat

#endif  // GW2COMBAT_COMBAT_LOOP_HPP
