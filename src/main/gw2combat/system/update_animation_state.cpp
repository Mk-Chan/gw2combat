#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/animation/is_animation_locked.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/skills/channeling_skill.hpp"
#include "gw2combat/component/skills/successful_skill_cast.hpp"

namespace gw2combat::system {

void update_animation_state(context& ctx) {
    ctx.registry.view<component::animation>().each([&](const entt::entity entity,
                                                       const component::animation& animation) {
        auto skill = animation.skill;
        bool has_quickness = ctx.registry.any_of<component::quickness>(entity);
        if (skill == skills::IDLE) {
            ctx.registry.remove<component::is_animation_locked>(entity);
            // if (entity == entt::entity{1}) {
            //     spdlog::info("tick: {}, idle", ctx.current_tick);
            // }
        } else if (ctx.current_tick >= animation.end_tick(has_quickness)) {
            spdlog::info("tick: {}, finished casting: {}", ctx.current_tick, skill.name);
            ctx.registry.remove<component::is_animation_locked>(entity);
            ctx.registry.remove<component::channeling_skill>(entity);
            ctx.registry.emplace<component::successful_skill_cast>(
                entity, component::successful_skill_cast{skill, ctx.current_tick});
            ctx.registry.remove<component::animation>(entity);
        } else {
            bool is_animation_locked = ctx.registry.any_of<component::is_animation_locked>(entity);
            if (!is_animation_locked) {
                spdlog::info("tick: {}, started casting: {}, end_tick: {}",
                             ctx.current_tick,
                             skill.name,
                             animation.end_tick(has_quickness));
                if (skill.type == skills::skill::type::CHANNELING_NO_AFTER_CAST ||
                    skill.type == skills::skill::type::CHANNELING_WITH_AFTER_CAST) {
                    ctx.registry.emplace<component::channeling_skill>(
                        entity, component::channeling_skill{skill, 0});
                }
                ctx.registry.emplace<component::is_animation_locked>(entity);
            }
        }
    });
}

}  // namespace gw2combat::system
