#include "system.hpp"

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/is_animation_locked.hpp"
#include "gw2combat/component/successful_skill_cast.hpp"

namespace gw2combat::system {

component::successful_skill_cast::skill get_skill_by_animation_state(
    component::animation::state animation) {
    if (animation == component::animation::state::CAST_SKILL_GUARDIAN_GREATSWORD_1_1) {
        return component::successful_skill_cast::skill::SKILL_GUARDIAN_GREATSWORD_1_1;
    } else if (animation == component::animation::CAST_SKILL_GUARDIAN_GREATSWORD_1_2) {
        return component::successful_skill_cast::skill::SKILL_GUARDIAN_GREATSWORD_1_2;
    } else if (animation == component::animation::CAST_SKILL_GUARDIAN_GREATSWORD_1_3) {
        return component::successful_skill_cast::skill::SKILL_GUARDIAN_GREATSWORD_1_3;
    } else {
        return component::successful_skill_cast::skill::NOTHING;
    }
}

void update_animation_lock_state(context& ctx) {
    ctx.registry.view<component::animation>().each(
        [&](const entt::entity entity, const component::animation& animation) {
            if (animation.current_state == component::animation::IDLE) {
                ctx.registry.remove<component::is_animation_locked>(entity);
            } else if (animation.accumulated_ticks >= animation.required_ticks_for_completion) {
                ctx.registry.remove<component::is_animation_locked>(entity);
                component::successful_skill_cast::skill skill =
                    get_skill_by_animation_state(animation.current_state);
                if (skill != component::successful_skill_cast::skill::NOTHING) {
                    ctx.registry.emplace<component::successful_skill_cast>(
                        entity, component::successful_skill_cast{skill});
                }
            } else {
                ctx.registry.emplace_or_replace<component::is_animation_locked>(entity);
            }
        });
}

}  // namespace gw2combat::system
