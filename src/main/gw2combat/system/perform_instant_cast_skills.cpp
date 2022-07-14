#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/skills/instant_cast_skills.hpp"

#include "gw2combat/component/skills/guardian/shield_of_wrath.hpp"

namespace gw2combat::system {

void perform_instant_cast_skills(context& ctx) {
    ctx.registry.view<component::instant_cast_skills>().each(
        [&](const entt::entity entity, const component::instant_cast_skills& instant_cast_skills) {
            for (const skills::skill& skill : instant_cast_skills.skills) {
                if (skill.name == "Shield of Wrath"_hs) {
                    spdlog::info("tick: {}, casting {}", ctx.current_tick, skill.name);
                    ctx.registry.emplace<component::shield_of_wrath>(
                        entity,
                        component::shield_of_wrath{ctx.current_tick +
                                                   skill.damage_start_after_cast_end});
                }
            }

            // Remove buffered skills after consumption
            ctx.registry.remove<component::instant_cast_skills>(entity);
        });
}

}  // namespace gw2combat::system
