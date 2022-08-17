#ifndef GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

#include "gw2combat/strike/strike.hpp"

namespace gw2combat::component {

struct outgoing_strikes_component {
    std::vector<strike::strike_t> strikes;
};

struct incoming_strikes_component {
    std::vector<strike::strike_t> strikes;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
