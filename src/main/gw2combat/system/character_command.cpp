#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/animation/is_animation_locked.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/no_more_rotation.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"

namespace gw2combat::system {

static inline size_t rotation_idx = 0;

void character_command(context& ctx) {
    ctx.registry.view<component::is_character>(entt::exclude<component::is_animation_locked>)
        .each([&](const entt::entity entity) {
            if (!ctx.registry.any_of<component::animation>(entity)) {
                ctx.registry.emplace<component::animation>(
                    entity,
                    component::animation{.skill = skills::IDLE, .start_tick = ctx.current_tick});
            }

            // For player 1, loop rotation
            if (entity == entt::entity{1}) {
                if (!ctx.predetermined_rotation ||
                    rotation_idx >= ctx.predetermined_rotation->skill_casts.size()) {
                    ctx.registry.emplace<component::no_more_rotation>(*singleton_entity);
                    return;
                }

                auto& skill_casts = ctx.predetermined_rotation->skill_casts;
                auto next_skill_cast = skill_casts[rotation_idx];
                auto next_skill = next_skill_cast.skill;
                auto cast_time = next_skill_cast.cast_time;
                if (ctx.current_tick < cast_time) {
                    ctx.registry.replace<component::animation>(
                        entity,
                        component::animation{.skill = skills::IDLE,
                                             .start_tick = ctx.current_tick});
                    return;
                }

                spdlog::info("tick: {}, next skill {}, error: {}",
                             ctx.current_tick,
                             next_skill.name,
                             (int)ctx.current_tick - (int)cast_time);
                if (next_skill.cast_duration[0] == 0) {
                    auto& instant_cast_skills =
                        ctx.registry.get_or_emplace<component::instant_cast_skills>(
                            entity, component::instant_cast_skills{});
                    instant_cast_skills.skills.push_back(next_skill);
                } else {
                    ctx.registry.replace<component::animation>(
                        entity,
                        component::animation{.skill = next_skill, .start_tick = ctx.current_tick});
                }

                // rotation_idx = (rotation_idx + 1) % skill_casts.size();
                ++rotation_idx;
            }
            // For every other character, loop idle animation
            else {
                ctx.registry.replace<component::animation>(
                    entity,
                    component::animation{.skill = skills::IDLE, .start_tick = ctx.current_tick});
            }
        });
}

}  // namespace gw2combat::system
