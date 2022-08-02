#ifndef GW2COMBAT_COMPONENT_DAMAGE_INCOMING_CONDITION_APPLICATION_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_INCOMING_CONDITION_APPLICATION_HPP

#include "gw2combat/effects.hpp"

namespace gw2combat::component {

struct incoming_condition_application {
    std::vector<effects::effect_application> effect_applications;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_INCOMING_CONDITION_APPLICATION_HPP
