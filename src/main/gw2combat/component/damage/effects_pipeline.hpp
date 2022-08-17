#ifndef GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP

#include "gw2combat/effect/application.hpp"

namespace gw2combat::component {

struct outgoing_effects_component {
    std::vector<effect::application_t> effect_applications;
};

struct incoming_effects_component {
    std::vector<effect::application_t> effect_applications;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP
