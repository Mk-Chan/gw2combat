#include "dispatch_strikes_and_effects.hpp"

#include "component/actor/relative_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/damage/effects_pipeline.hpp"
#include "component/damage/strikes_pipeline.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/hierarchy/owner_component.hpp"

#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"

namespace gw2combat::system {

void dispatch_strikes(registry_t& registry) {
    registry.view<component::team, component::outgoing_strikes_component>().each(
        [&](entity_t source_entity,
            const component::team& source_team,
            const component::outgoing_strikes_component& outgoing_strikes_component) {
            for (component::strike_t this_strike : outgoing_strikes_component.strikes) {
                registry.view<component::team>(entt::exclude<component::owner_component>)
                    .each([&](entity_t other_entity, const component::team& other_team) {
                        if (this_strike.num_targets <= 0 || source_team.id == other_team.id) {
                            return;
                        }
                        auto& incoming_strikes_component =
                            registry.get_or_emplace<component::incoming_strikes_component>(
                                other_entity);

                        entity_t strike_source_entity = utils::get_owner(source_entity, registry);
                        auto& strike_source_relative_attributes =
                            registry.get<component::relative_attributes>(strike_source_entity);

                        double critical_chance_multiplier = std::min(
                            strike_source_relative_attributes.get(
                                other_entity, actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER),
                            1.0);
                        bool is_critical = this_strike.can_critical_strike &&
                                           (critical_chance_multiplier == 1.0 ||
                                            utils::check_random_success(utils::round_down(
                                                100.0 * critical_chance_multiplier)));
                        incoming_strikes_component.strikes.emplace_back(component::incoming_strike{
                            strike_source_entity, this_strike, is_critical});
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
            entity_t actual_source_entity = [&]() {
                if (registry.any_of<component::is_effect, component::is_unique_effect>(
                        source_entity)) {
                    return registry.get<component::source_actor>(source_entity).entity;
                }
                return utils::get_owner(source_entity, registry);
            }();
            for (component::effect_application_t application :
                 outgoing_effects_component.effect_applications) {
                if (application.direction == component::effect_application_t::direction_t::SELF) {
                    if (!utils::independent_conditions_satisfied(application.condition,
                                                                 actual_source_entity,
                                                                 actual_source_entity,
                                                                 registry)
                             .satisfied) {
                        continue;
                    }

                    auto& incoming_effects_component =
                        registry.get_or_emplace<component::incoming_effects_component>(
                            actual_source_entity);
                    incoming_effects_component.effect_applications.emplace_back(
                        component::incoming_effect_application{source_entity, application});
                } else if (application.direction ==
                           component::effect_application_t::direction_t::TEAM) {
                    auto& incoming_effects_component =
                        registry.get_or_emplace<component::incoming_effects_component>(
                            actual_source_entity);
                    incoming_effects_component.effect_applications.emplace_back(
                        component::incoming_effect_application{source_entity, application});
                    --application.num_targets;

                    registry.view<component::team>(entt::exclude<component::owner_component>)
                        .each([&](entity_t other_entity, const component::team& other_team) {
                            if (application.num_targets <= 0 || other_entity == source_entity ||
                                other_team.id != source_team.id ||
                                !utils::independent_conditions_satisfied(application.condition,
                                                                         actual_source_entity,
                                                                         other_entity,
                                                                         registry)
                                     .satisfied) {
                                return;
                            }

                            auto& incoming_effects_component =
                                registry.get_or_emplace<component::incoming_effects_component>(
                                    other_entity);
                            incoming_effects_component.effect_applications.emplace_back(
                                component::incoming_effect_application{source_entity, application});
                            --application.num_targets;
                        });
                } else if (application.direction ==
                           component::effect_application_t::direction_t::OUTGOING) {
                    registry.view<component::team>(entt::exclude<component::owner_component>)
                        .each([&](entity_t other_entity, const component::team& other_team) {
                            if (application.num_targets <= 0 || other_team.id == source_team.id ||
                                !utils::independent_conditions_satisfied(application.condition,
                                                                         actual_source_entity,
                                                                         other_entity,
                                                                         registry)
                                     .satisfied) {
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
