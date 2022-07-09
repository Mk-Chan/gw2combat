#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/is_animation_locked.hpp"
#include "gw2combat/component/is_character.hpp"

namespace gw2combat::system {

void character_command(context& ctx) {
    ctx.registry.view<component::is_character>(entt::exclude<component::is_animation_locked>)
        .each([&](const entt::entity entity) {
            auto& animation = ctx.registry.get_or_emplace<component::animation>(
                entity,
                component::animation{.current_state = component::animation::IDLE,
                                     .start_tick = ctx.current_tick,
                                     .accumulated_ticks = tick_t{0},
                                     .required_ticks_for_completion = tick_t{1000}});

            // For player 1, loop aa-chain
            if (entity == entt::entity{1}) {
                // If 1 is pressed and previous animation was aa2 and has ended, queue aa3
                if (animation.current_state ==
                    component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_2) {
                    animation.current_state =
                        component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_3;
                    animation.start_tick = ctx.current_tick;
                    animation.accumulated_ticks = tick_t{0};
                    animation.required_ticks_for_completion = tick_t{600};
                }
                // If 1 is pressed and previous animation was aa1 and has ended, queue aa2
                else if (animation.current_state ==
                         component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_1) {
                    animation.current_state =
                        component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_2;
                    animation.start_tick = ctx.current_tick;
                    animation.accumulated_ticks = tick_t{0};
                    animation.required_ticks_for_completion = tick_t{870};
                }
                // If previous state was idle (or actually any skill other than aa-chain skill),
                // queue aa1
                else {
                    animation.current_state =
                        component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_1;
                    animation.start_tick = ctx.current_tick;
                    animation.accumulated_ticks = tick_t{0};
                    animation.required_ticks_for_completion = tick_t{1'020};
                }
            }
            // For every other character, loop idle animation
            else {
                animation.current_state = component::animation::state::IDLE;
                animation.start_tick = ctx.current_tick;
                animation.accumulated_ticks = tick_t{0};
                animation.required_ticks_for_completion = tick_t{1'000};
            }
        });
}

}  // namespace gw2combat::system
