#include "gw2combat/system/system.hpp"

#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/incoming_condition_application.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"

namespace gw2combat::system {

void incoming_condition_detection(registry_t& registry, tick_t current_tick) {
    registry.view<component::source_entity, component::outgoing_condition_application>().each(
        [&](const component::source_entity& source_entity,
            const component::outgoing_condition_application& outgoing_condition_application) {
            auto targeting_ptr = registry.try_get<component::targeting>(
                utils::get_source_entity(source_entity.entity, registry));
            if (!targeting_ptr) {
                return;
            }
            if (!outgoing_condition_application.effect_applications.empty()) {
                auto& target_incoming_condition_application =
                    registry.get_or_emplace<component::incoming_condition_application>(
                        targeting_ptr->entity);
                for (auto& effect_application :
                     outgoing_condition_application.effect_applications) {
                    spdlog::info("tick: {}, entity: {}, added {} {} effects, {} duration",
                                 current_tick,
                                 utils::get_entity_name(targeting_ptr->entity, registry),
                                 effect_application.num_stacks,
                                 nlohmann::json{effect_application.effect_type}[0],
                                 effect_application.duration);
                    target_incoming_condition_application.effect_applications.emplace_back(
                        effect_application);
                }
            }
        });
    registry.view<component::targeting, component::outgoing_condition_application>().each(
        [&](const component::targeting& targeting,
            const component::outgoing_condition_application& outgoing_condition_application) {
            if (!outgoing_condition_application.effect_applications.empty()) {
                auto& target_incoming_condition_application =
                    registry.get_or_emplace<component::incoming_condition_application>(
                        targeting.entity);
                for (auto& effect_application :
                     outgoing_condition_application.effect_applications) {
                    spdlog::info("tick: {}, entity: {}, added {} {} effects, {} duration",
                                 current_tick,
                                 utils::get_entity_name(targeting.entity, registry),
                                 effect_application.num_stacks,
                                 nlohmann::json{effect_application.effect_type}[0],
                                 effect_application.duration);
                    target_incoming_condition_application.effect_applications.emplace_back(
                        effect_application);
                }
            }
        });
}

}  // namespace gw2combat::system
