#ifndef GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP

#include "gw2combat/strike/strike.hpp"

namespace gw2combat::component {

struct incoming_strike {
    entity_t source_entity;
    strike::strike_t strike;
};

struct outgoing_strikes_component {
    std::vector<strike::strike_t> strikes;
};

struct incoming_strikes_component {
    std::vector<incoming_strike> strikes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strike, source_entity, strike)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_strikes_component, strikes)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_strikes_component, strikes)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_STRIKES_PIPELINE_HPP
