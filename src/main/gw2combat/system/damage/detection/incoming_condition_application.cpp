#include <spdlog/spdlog.h>

#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/effect_components.hpp"

namespace gw2combat::system {

void incoming_condition_application(registry_t& registry, tick_t current_tick) {
    registry.view<component::targeting, component::outgoing_condition_application>().each(
        [&](const component::targeting& targeting,
            const component::outgoing_condition_application& outgoing_condition_application) {
            if (!outgoing_condition_application.burning_effects.empty()) {
                auto& target_burning = registry.get_or_emplace<component::burning>(
                    targeting.entity, component::burning{});
                for (auto& burning_effect : outgoing_condition_application.burning_effects) {
                    target_burning.effect.add(burning_effect);
                }
                spdlog::info("tick: {}, entity: {}, added {} burning effects",
                             current_tick,
                             utils::get_name(targeting.entity, registry),
                             outgoing_condition_application.burning_effects.size());
            }
            if (!outgoing_condition_application.bleeding_effects.empty()) {
                auto& target_bleeding = registry.get_or_emplace<component::bleeding>(
                    targeting.entity, component::bleeding{});
                for (auto& bleeding_effect : outgoing_condition_application.bleeding_effects) {
                    target_bleeding.effect.add(bleeding_effect);
                }
                spdlog::info("tick: {}, entity: {}, added {} bleeding effects",
                             current_tick,
                             utils::get_name(targeting.entity, registry),
                             outgoing_condition_application.bleeding_effects.size());
            }
            if (outgoing_condition_application.binding_blade_effect) {
                registry.emplace_or_replace<component::binding_blade>(
                    targeting.entity,
                    component::binding_blade{*outgoing_condition_application.binding_blade_effect});
                spdlog::info("tick: {}, entity: {}, added binding blade effect",
                             current_tick,
                             utils::get_name(targeting.entity, registry));
            }
            // NOTE: Torment, confusion etc...
        });
}

}  // namespace gw2combat::system
