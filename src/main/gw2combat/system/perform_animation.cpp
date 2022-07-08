#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/animation.hpp"
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

void perform_animation(context& ctx) {
    ctx.registry.view<component::animation>().each([&](const entt::entity entity,
                                                       const component::animation& animation) {
        // NOTE: Code for skills which have some sort of effect during cast animation here

        // Code to detect animation end
        if (animation.accumulated_ticks >= animation.required_ticks_for_completion ||
            animation.current_state == component::animation::IDLE) {
            component::successfully_cast_skill::skill skill =
                get_skill_by_animation_state(animation.current_state);
            if (skill != component::successfully_cast_skill::skill::NOTHING) {
                ctx.registry.emplace<component::successfully_cast_skill>(
                    entity, component::successfully_cast_skill{skill});
                // NOTE: This is here because only guardian gs aa-chain is implemented for now
                auto& virtue_of_justice = ctx.registry.get<component::virtue_of_justice>(entity);
                // Maybe += number of targets hit? Well, for now only single target is supported
                // every 3 hits because of permeating wrath
                virtue_of_justice.consecutive_successful_hits =
                    (virtue_of_justice.consecutive_successful_hits % 3) + 1;
                if (virtue_of_justice.consecutive_successful_hits == 3) {
                    virtue_of_justice.apply_burning_on_tick = ctx.current_tick;
                }
            }
        }

        // spdlog::info("entity: {}, current animation: {}",
        //              static_cast<std::uint32_t>(entity),
        //              animation.current_state);
    });
}

}  // namespace gw2combat::system
