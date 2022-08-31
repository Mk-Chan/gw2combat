#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/component/actor/team.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/strikes_pipeline.hpp"
#include "gw2combat/component/owner_actor.hpp"

namespace gw2combat::system {

void dispatch_strikes(registry_t& registry) {
    registry.view<component::team, component::outgoing_strikes_component>().each(
        [&](entity_t source_entity,
            const component::team& source_team,
            const component::outgoing_strikes_component& outgoing_strikes_component) {
            for (strike::strike_t this_strike : outgoing_strikes_component.strikes) {
                registry.view<component::team>(entt::exclude<component::owner_actor>)
                    .each([&](entity_t other_entity, const component::team& other_team) {
                        if (this_strike.num_targets <= 0 || source_team.id == other_team.id) {
                            return;
                        }
                        auto& incoming_strikes_component =
                            registry.get_or_emplace<component::incoming_strikes_component>(
                                other_entity);
                        incoming_strikes_component.strikes.emplace_back(
                            component::incoming_strike{source_entity, this_strike});
                        --this_strike.num_targets;
                    });
            }
        });
}

void dispatch_effects(registry_t& registry) {
    registry.view<component::team, component::outgoing_effects_component>().each(
        [&](entity_t source_entity,
            const component::team& source_team,
            const component::outgoing_effects_component& outgoing_effects_component) {
            for (effect::application_t application :
                 outgoing_effects_component.effect_applications) {
                if (application.direction == effect::direction_t::SELF) {
                    auto& incoming_effects_component =
                        registry.get_or_emplace<component::incoming_effects_component>(
                            source_entity);
                    incoming_effects_component.effect_applications.emplace_back(
                        component::incoming_effect_application{source_entity, application});
                } else if (application.direction == effect::direction_t::TEAM) {
                    auto& incoming_effects_component =
                        registry.get_or_emplace<component::incoming_effects_component>(
                            source_entity);
                    incoming_effects_component.effect_applications.emplace_back(
                        component::incoming_effect_application{source_entity, application});
                    --application.num_targets;

                    registry.view<component::team>(entt::exclude<component::owner_actor>)
                        .each([&](entity_t other_entity, const component::team& other_team) {
                            if (application.num_targets <= 0 || other_entity == source_entity ||
                                other_team.id != source_team.id) {
                                return;
                            }
                            auto& incoming_effects_component =
                                registry.get_or_emplace<component::incoming_effects_component>(
                                    other_entity);
                            incoming_effects_component.effect_applications.emplace_back(
                                component::incoming_effect_application{source_entity, application});
                            --application.num_targets;
                        });
                } else if (application.direction == effect::direction_t::OUTGOING) {
                    registry.view<component::team>(entt::exclude<component::owner_actor>)
                        .each([&](entity_t other_entity, const component::team& other_team) {
                            if (application.num_targets <= 0 || other_team.id == source_team.id) {
                                return;
                            }
                            auto& incoming_effects_component =
                                registry.get_or_emplace<component::incoming_effects_component>(
                                    other_entity);
                            incoming_effects_component.effect_applications.emplace_back(
                                component::incoming_effect_application{source_entity, application});
                            --application.num_targets;
                        });
                }
            }
        });
}

}  // namespace gw2combat::system
