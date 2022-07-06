#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/character_input.hpp"
#include "gw2combat/component/profession/guardian/virtue_of_justice.hpp"
#include "gw2combat/component/successfully_cast_skill.hpp"

namespace gw2combat::system {

component::successfully_cast_skill::skill get_skill_by_animation_state(
    component::animation::state animation) {
    if (animation == component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_1) {
        return component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_1;
    } else if (animation == component::animation::CAST_SKILL_GUARDIAN_GREATSWORD_1_2) {
        return component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_2;
    } else if (animation == component::animation::CAST_SKILL_GUARDIAN_GREATSWORD_1_3) {
        return component::successfully_cast_skill::skill::SKILL_GUARDIAN_GREATSWORD_1_3;
    } else {
        return component::successfully_cast_skill::skill::NOTHING;
    }
}

void character_animation(context& ctx) {
    ctx.registry.view<component::character_input>().each(
        [&](const entt::entity entity, const component::character_input& character_input) {
            auto& animation = ctx.registry.get_or_emplace<component::animation>(
                entity,
                component::animation{.current_state = component::animation::IDLE,
                                     .start_tick = ctx.current_tick,
                                     .accumulated_ticks = 0,
                                     .required_ticks_for_completion = tick_t{1'000}});

            bool is_not_animation_locked = false;
            if (animation.accumulated_ticks >= animation.required_ticks_for_completion ||
                animation.current_state == component::animation::IDLE) {
                component::successfully_cast_skill::skill skill =
                    get_skill_by_animation_state(animation.current_state);
                if (skill != component::successfully_cast_skill::skill::NOTHING) {
                    ctx.registry.emplace<component::successfully_cast_skill>(
                        entity, component::successfully_cast_skill{skill});
                    // NOTE: This is here because only guardian gs aa-chain is implemented for now
                    auto& virtue_of_justice =
                        ctx.registry.get<component::virtue_of_justice>(entity);
                    // Maybe += number of targets hit? Well, for now only single target is supported
                    // every 3 hits because of permeating wrath
                    virtue_of_justice.consecutive_successful_hits =
                        (virtue_of_justice.consecutive_successful_hits % 3) + 1;
                    if (virtue_of_justice.consecutive_successful_hits == 3) {
                        virtue_of_justice.apply_burning_on_tick = ctx.current_tick;
                    }
                }
                is_not_animation_locked = true;
            }

            // Restart idle animation if nothing is pressed when it ends and previous animation is
            // over
            if (character_input.input_bitset == component::character_input::command::NOTHING &&
                animation.current_state != component::animation::IDLE && is_not_animation_locked) {
                animation.current_state = component::animation::state::IDLE;
                animation.start_tick = ctx.current_tick;
                animation.accumulated_ticks = tick_t{0};
                animation.required_ticks_for_completion = tick_t{1'000};
            }
            // If 1 is pressed, swap to it only if idle or previous animation ended
            else if (is_not_animation_locked &&
                     character_input.contains(
                         {component::character_input::command::WEAPON_SKILL_1})) {
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

            // spdlog::info("entity: {}, current animation: {}",
            //              static_cast<std::uint32_t>(entity),
            //              animation.current_state);
        });
}

}  // namespace gw2combat::system
