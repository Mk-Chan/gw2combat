#ifndef GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP

#include "gw2combat/effect/application.hpp"

namespace gw2combat::component {

struct incoming_effect_application {
    entity_t source_entity;
    effect::application_t effect_application;
};

struct outgoing_effects_component {
    std::vector<effect::application_t> effect_applications;
};

struct incoming_effects_component {
    std::vector<incoming_effect_application> effect_applications;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_effect_application,
                                                source_entity,
                                                effect_application)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_effects_component, effect_applications)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_effects_component, effect_applications)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_EFFECTS_PIPELINE_HPP
