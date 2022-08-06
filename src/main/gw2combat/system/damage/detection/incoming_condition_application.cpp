#include "gw2combat/system/system.hpp"

#include "gw2combat/component/damage/incoming_condition_application.hpp"

namespace gw2combat::system {

void incoming_condition_application(registry_t& registry, tick_t current_tick) {
    registry.view<component::incoming_condition_application, component::effects_component>().each(
        [&](const component::incoming_condition_application& incoming_condition_application,
            component::effects_component& effects_component) {
            for (auto& effect_application : incoming_condition_application.effect_applications) {
                utils::apply_effects(effect_application, effects_component);
            }
        });
}

}  // namespace gw2combat::system
