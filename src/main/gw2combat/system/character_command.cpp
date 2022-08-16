#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/no_more_rotation.hpp"
#include "gw2combat/component/character/rotation.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"

namespace gw2combat::system {

void get_available_skills(entity_t entity, registry_t& registry, tick_t current_tick) {

}

void character_command(registry_t& registry, tick_t current_tick) {
    registry.view<component::is_actor>(entt::exclude<component::no_more_rotation>)
        .each([&](entity_t entity) {
            get_available_skills(entity, registry, current_tick);

            auto rotation_ptr = registry.try_get<component::rotation>(entity);
            if (!rotation_ptr || rotation_ptr->current_idx >= rotation_ptr->skill_casts.size()) {
                registry.emplace_or_replace<component::no_more_rotation>(entity);
                return;
            }

            auto& skill_casts = rotation_ptr->skill_casts;
            auto next_skill_cast = skill_casts[rotation_ptr->current_idx];
            auto next_skill = next_skill_cast.skill;
            auto cast_time = next_skill_cast.cast_time;
            if (current_tick < cast_time) {
                return;  // NOTE: Comment this to remove any idle time and perfectly perform skills
            }

            if (next_skill.cast_duration[0] == 0) {
                spdlog::info("tick: {}, entity: {}, insta-command: {}",
                             current_tick,
                             utils::get_entity_name(entity, registry),
                             next_skill.name);
                auto& instant_cast_skills = registry.get_or_emplace<component::instant_cast_skills>(
                    entity, component::instant_cast_skills{});
                instant_cast_skills.skills.push_back(next_skill);
                ++rotation_ptr->current_idx;
            } else if (!registry.any_of<component::normal_cast_skill>(entity)) {
                spdlog::info("tick: {}, entity: {}, command: {}",
                             current_tick,
                             utils::get_entity_name(entity, registry),
                             next_skill.name);
                registry.emplace<component::normal_cast_skill>(
                    entity, component::normal_cast_skill{next_skill});
                ++rotation_ptr->current_idx;
            }
        });
}

}  // namespace gw2combat::system
