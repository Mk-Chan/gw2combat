#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/animation/is_animation_locked.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
#include "gw2combat/component/skills/guardian/sword_of_justice.hpp"

namespace gw2combat::system {

void sword_of_justice(context& ctx) {
    ctx.registry.view<component::sword_of_justice>().each(
        [&](const entt::entity entity, const component::sword_of_justice& sword_of_justice) {
            if (ctx.current_tick >=
                sword_of_justice.start_tick + component::sword_of_justice::duration) {
                ctx.registry.destroy(entity);
                spdlog::info("tick: {}, entity: {}, sword of justice disappeared",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            } else if (ctx.current_tick >=
                           sword_of_justice.start_tick +
                               component::sword_of_justice::damage_start_after_spawn &&
                       !ctx.registry.any_of<component::is_animation_locked>(entity)) {
                auto sword_of_justice_attack_skill =
                    skills::get_by_name("Sword of Justice Attack"_hs);

                ctx.registry.emplace<component::outgoing_damage_source>(
                    entity, component::outgoing_damage_source{sword_of_justice.source});
                ctx.registry.emplace<component::animation>(
                    entity, component::animation{sword_of_justice_attack_skill, ctx.current_tick});
                spdlog::info("tick: {}, entity: {}, sword of justice started attacking",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system
