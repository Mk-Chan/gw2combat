#include "hooks.hpp"

#include "common.hpp"

#include "component/damage/effects_pipeline.hpp"
#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"

#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/side_effect_utils.hpp"

namespace gw2combat::system {

void on_strike_hooks(registry_t& registry) {
    // NOTE: Also make the on pulse effect applications a hook in a separate system, then also
    //       add effect application as a side_effect.
    registry.view<component::incoming_strikes_component>(entt::exclude<component::owner_component>)
        .each([&](entity_t target_entity,
                  const component::incoming_strikes_component& incoming_strikes_component) {
            for (const auto& strike : incoming_strikes_component.strikes) {
                auto& skill_configuration =
                    registry.get<component::is_skill>(strike.strike.skill_entity)
                        .skill_configuration;

                // NOTE: Extreme hack to avoid coding a whole new type of damage just for food.
                //       Implement properly if there are more such instances!
                if (skill_configuration.skill_key == "Lifesteal Proc") {
                    continue;
                }

                entity_t strike_source_entity = utils::get_owner(strike.source_entity, registry);
                auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                    return utils::on_strike_conditions_satisfied(condition,
                                                                 strike_source_entity,
                                                                 target_entity,
                                                                 strike.is_critical,
                                                                 strike.strike,
                                                                 skill_configuration,
                                                                 registry);
                };
                utils::apply_side_effects(registry, strike_source_entity, side_effect_condition_fn);

                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(
                        strike_source_entity);
                std::transform(
                    strike.strike.on_strike_effect_applications.begin(),
                    strike.strike.on_strike_effect_applications.end(),
                    std::back_inserter(outgoing_effects_component.effect_applications),
                    [&](const configuration::effect_application_t& effect_application) {
                        return component::effect_application_t{
                            .condition = effect_application.condition,
                            .source_skill = skill_configuration.skill_key,
                            .effect = effect_application.effect,
                            .unique_effect = effect_application.unique_effect,
                            .direction = component::effect_application_t::convert_direction(
                                effect_application.direction),
                            .base_duration_ms = effect_application.base_duration_ms,
                            .num_stacks = effect_application.num_stacks,
                            .num_targets = effect_application.num_targets};
                    });
            }
        });
}

void on_ammo_gained_hooks(registry_t& registry) {
    registry.view<component::is_skill, component::ammo_gained>().each(
        [&](entity_t skill_entity, const component::is_skill& is_skill) {
            auto actor_entity = utils::get_owner(skill_entity, registry);
            auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                return utils::on_ammo_gain_conditions_satisfied(
                    condition, actor_entity, is_skill.skill_configuration, registry);
            };
            utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
        });
}
void on_begun_casting_skills_hooks(registry_t& registry) {
    registry.view<component::begun_casting_skills>().each(
        [&](entity_t actor_entity, component::begun_casting_skills& begun_casting_skills) {
            for (auto casting_skill_entity : begun_casting_skills.skill_entities) {
                auto& skill_configuration =
                    registry.get<component::is_skill>(casting_skill_entity).skill_configuration;
                auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                    return utils::on_begun_casting_conditions_satisfied(
                        condition, actor_entity, skill_configuration, registry);
                };
                utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
            }
        });
}
void on_every_tick_hooks(registry_t& registry) {
    registry.view<component::is_actor>(entt::exclude<component::owner_component>)
        .each([&](entity_t actor_entity) {
            auto side_effect_condition_fn = [&](const configuration::condition_t& condition) {
                return utils::independent_conditions_satisfied(
                           condition, actor_entity, std::nullopt, registry)
                    .satisfied;
            };
            utils::apply_side_effects(registry, actor_entity, side_effect_condition_fn);
        });
}

}  // namespace gw2combat::system
