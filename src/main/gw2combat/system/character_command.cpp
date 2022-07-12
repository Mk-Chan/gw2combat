#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/animation/is_animation_locked.hpp"
#include "gw2combat/component/character/is_character.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"

namespace gw2combat::system {

static inline size_t rotation_idx = 0;
static inline std::array rotation{
    //skills::GUARDIAN_FOCUS_5,
    skills::GUARDIAN_GREATSWORD_2,

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
    skills::GUARDIAN_GREATSWORD_1_3,
};

void character_command(context& ctx) {
    ctx.registry.view<component::is_character>(entt::exclude<component::is_animation_locked>)
        .each([&](const entt::entity entity) {
            auto& animation = ctx.registry.get_or_emplace<component::animation>(
                entity,
                component::animation{.name = component::animation::IDLE,
                                     .start_tick = ctx.current_tick,
                                     .required_ticks_for_completion = skills::IDLE.cast_duration});

            // For player 1, loop rotation
            if (entity == entt::entity{1}) {
                bool queued_non_instant_cast_skill = false;
                while (true) {
                    auto next_skill = rotation[rotation_idx];
                    if (next_skill.cast_duration[0] > 0) {
                        if (queued_non_instant_cast_skill) {
                            break;
                        } else {
                            ctx.registry.replace<component::animation>(
                                entity,
                                component::animation{
                                    .name = next_skill.name,
                                    .start_tick = ctx.current_tick,
                                    .required_ticks_for_completion = next_skill.cast_duration});
                            queued_non_instant_cast_skill = true;
                        }
                    } else {
                        auto& instant_cast_skills =
                            ctx.registry.get_or_emplace<component::instant_cast_skills>(
                                entity, component::instant_cast_skills{});
                        instant_cast_skills.skills.push_back(next_skill);
                    }
                    rotation_idx = (rotation_idx + 1) % rotation.size();
                }
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
