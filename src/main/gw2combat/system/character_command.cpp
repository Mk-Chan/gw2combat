#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/is_animation_locked.hpp"
#include "gw2combat/component/is_character.hpp"

namespace gw2combat::system {

static inline size_t skill_idx = 0;
static inline std::array skills_in_order{skills::GUARDIAN_GREATSWORD_2,

                                         skills::GUARDIAN_GREATSWORD_1_1,
                                         skills::GUARDIAN_GREATSWORD_1_2,
                                         skills::GUARDIAN_GREATSWORD_1_3,

                                         skills::GUARDIAN_GREATSWORD_1_1,
                                         skills::GUARDIAN_GREATSWORD_1_2,
                                         skills::GUARDIAN_GREATSWORD_1_3,

                                         skills::GUARDIAN_GREATSWORD_1_1,
                                         skills::GUARDIAN_GREATSWORD_1_2,
                                         skills::GUARDIAN_GREATSWORD_1_3,

                                         skills::GUARDIAN_GREATSWORD_1_1,
                                         skills::GUARDIAN_GREATSWORD_1_2,
                                         skills::GUARDIAN_GREATSWORD_1_3};

void character_command(context& ctx) {
    ctx.registry.view<component::is_character>(entt::exclude<component::is_animation_locked>)
        .each([&](const entt::entity entity) {
            auto& animation = ctx.registry.get_or_emplace<component::animation>(
                entity,
                component::animation{.name = component::animation::IDLE,
                                     .start_tick = ctx.current_tick,
                                     .required_ticks_for_completion = skills::IDLE.cast_duration});

            // For player 1, loop aa-chain
            if (entity == entt::entity{1}) {
                auto next_skill = skills_in_order[skill_idx];
                ctx.registry.replace<component::animation>(
                    entity,
                    component::animation{
                        .name = next_skill.name,
                        .start_tick = ctx.current_tick,
                        .required_ticks_for_completion = next_skill.cast_duration});
                skill_idx = (skill_idx + 1) % skills_in_order.size();
            }
            // For every other character, loop idle animation
            else {
                ctx.registry.replace<component::animation>(
                    entity,
                    component::animation{
                        .name = component::animation::IDLE,
                        .start_tick = ctx.current_tick,
                        .required_ticks_for_completion = skills::IDLE.cast_duration});
            }
        });
}

}  // namespace gw2combat::system
