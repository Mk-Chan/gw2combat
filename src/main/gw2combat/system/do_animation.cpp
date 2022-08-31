#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/quickness.hpp"
#include "gw2combat/component/skill/finished_animation.hpp"

namespace gw2combat::system {

void check_if_animation_finished(registry_t& registry) {
    registry.view<component::animation>().each(
        [&](entity_t entity, component::animation& animation) {
            if (animation.duration[0] == 0) {
                registry.emplace<component::finished_animation>(
                    entity, component::finished_animation{animation.skill});
                return;
            }

            int no_quickness_progress_pct = animation.progress[0] * 100 / animation.duration[0];
            int quickness_progress_pct = animation.progress[1] * 100 / animation.duration[1];

            if (no_quickness_progress_pct + quickness_progress_pct >= 100) {
                registry.emplace<component::finished_animation>(
                    entity, component::finished_animation{animation.skill});
                // spdlog::info("[{}] finished animation for skill {}",
                //              utils::get_current_tick(registry),
                //              utils::to_string(animation.skill));
                return;
            }
        });
}

void progress_animations(registry_t& registry) {
    registry
        .view<component::animation>(
            entt::exclude<component::quickness, component::finished_animation>)
        .each([&](component::animation& animation) { ++animation.progress[0]; });
    registry
        .view<component::animation, component::quickness>(
            entt::exclude<component::finished_animation>)
        .each([&](component::animation& animation) { ++animation.progress[1]; });
}

void remove_animation_if_finished(registry_t& registry) {
    registry.view<component::animation, component::finished_animation>().each(
        [&](entity_t entity, const component::animation&, const component::finished_animation&) {
            registry.remove<component::animation>(entity);
        });
}

}  // namespace gw2combat::system
