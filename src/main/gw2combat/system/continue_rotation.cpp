#include "system.hpp"

#include "gw2combat/utilities/base_utilities.hpp"
#include "gw2combat/utilities/logging_utilities.hpp"

#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/no_more_rotation.hpp"
#include "gw2combat/component/actor/rotation_component.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"
#include "gw2combat/utilities/skill_utilities.hpp"

namespace gw2combat::system {

void continue_rotation(registry_t& registry, bool enable_checks) {
    registry
        .view<component::rotation_component>(
            entt::exclude<component::animation, component::no_more_rotation>)
        .each([&](entity_t entity, component::rotation_component& rotation_component) {
            auto current_tick = utils::get_current_tick(registry);

            if (rotation_component.current_idx >= rotation_component.rotation.skill_casts.size()) {
                if (rotation_component.repeat) {
                    rotation_component.current_idx = 0;
                    rotation_component.offset = current_tick;
                } else {
                    registry.emplace<component::no_more_rotation>(entity);
                    return;
                }
            }

            auto& next_skill_cast =
                rotation_component.rotation.skill_casts[rotation_component.current_idx];
            auto next_skill = next_skill_cast.skill;

            if (current_tick < next_skill_cast.cast_time_ms + rotation_component.offset) {
                // return;
            }

            if (enable_checks) {
                if (!utils::actor_has_skill(entity, next_skill, registry)) {
                    throw std::runtime_error(fmt::format("{} does not have skill {}",
                                                         utils::get_entity_name(entity, registry),
                                                         utils::to_string(next_skill)));
                }
                if (!utils::can_cast_skill(entity, next_skill, registry)) {
                    // utils::skill_has_cooldown(entity, next_skill, registry) ||
                    // utils::skill_has_recharge(entity, next_skill, registry)) {
                    utils::log_component<component::cooldown>(registry);
                    throw std::runtime_error(fmt::format("{} cannot cast skill {}",
                                                         utils::get_entity_name(entity, registry),
                                                         utils::to_string(next_skill)));
                } else if (!registry.any_of<component::bundle_component>(entity)) {
                    utils::put_skill_on_cooldown(entity, next_skill, registry);
                }
            }

            spdlog::info("[{}] {} casting skill {}",
                         utils::get_current_tick(registry),
                         utils::get_entity_name(entity, registry),
                         utils::to_string(next_skill));
            registry.emplace<component::animation>(
                entity,
                component::animation{
                    next_skill,
                    registry.get<actor::skill_database>(utils::get_owner(entity, registry))
                        .find_by(next_skill)
                        .cast_duration});

            rotation_component.current_idx += 1;
        });
}

}  // namespace gw2combat::system
