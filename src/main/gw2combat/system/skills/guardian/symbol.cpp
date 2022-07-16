#include "gw2combat/system/system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/skills.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/animation/is_animation_locked.hpp"
#include "gw2combat/component/character/targeting.hpp"
#include "gw2combat/component/damage/outgoing_damage_source.hpp"
#include "gw2combat/component/skills/guardian/symbol.hpp"

namespace gw2combat::system {

void symbol(context& ctx) {
    ctx.registry.view<component::symbol>().each(
        [&](const entt::entity entity, const component::symbol& symbol) {
            if (ctx.current_tick >= symbol.start_tick + symbol.skill.cast_duration[0]) {
                ctx.registry.destroy(entity);
                spdlog::info("tick: {}, entity: {}, symbol disappeared",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            } else if (ctx.current_tick >= symbol.start_tick + symbol.skill.damage_start &&
                       !ctx.registry.any_of<component::is_animation_locked>(entity)) {
                ctx.registry.emplace<component::outgoing_damage_source>(
                    entity, component::outgoing_damage_source{symbol.source});
                ctx.registry.emplace<component::animation>(
                    entity, component::animation{symbol.skill, ctx.current_tick});
                spdlog::info("tick: {}, entity: {}, symbol started attacking",
                             ctx.current_tick,
                             static_cast<std::uint32_t>(entity));
            }
        });
}

}  // namespace gw2combat::system
