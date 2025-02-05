#include "hooks.hpp"

#include "common.hpp"

#include "component/actor/begun_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"

#include "utils/condition_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/side_effect_utils.hpp"

namespace gw2combat::system {

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
