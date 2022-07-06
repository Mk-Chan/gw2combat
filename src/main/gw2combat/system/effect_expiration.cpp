#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/boon/aegis.hpp"
#include "gw2combat/component/boon/fury.hpp"
#include "gw2combat/component/boon/might.hpp"
#include "gw2combat/component/boon/quickness.hpp"
#include "gw2combat/component/boon/resolution.hpp"
#include "gw2combat/component/character_input.hpp"
#include "gw2combat/component/condition/burning.hpp"
#include "gw2combat/component/condition/vulnerability.hpp"

namespace gw2combat::system {

void effect_expiration(context& ctx) {
    ctx.registry.view<component::character_input>().each([&](const entt::entity entity,
                                                             const component::character_input&) {
        // Boons
        auto might = ctx.registry.try_get<component::might>(entity);
        auto fury = ctx.registry.try_get<component::fury>(entity);
        auto quickness = ctx.registry.try_get<component::quickness>(entity);
        auto resolution = ctx.registry.try_get<component::resolution>(entity);
        auto aegis = ctx.registry.try_get<component::aegis>(entity);

        if (might) {
            while (!might->stacks.empty() && ctx.current_tick > might->stacks.top().end_time) {
                might->stacks.pop();
            }
            if (might->stacks.empty()) {
                ctx.registry.remove<component::might>(entity);
            }
        }
        if (fury) {
        }
        if (fury && ctx.current_tick > fury->stack.end_time) {
            ctx.registry.remove<component::fury>(entity);
        }
        if (quickness && ctx.current_tick > quickness->stack.end_time) {
            ctx.registry.remove<component::quickness>(entity);
        }
        if (resolution && ctx.current_tick > resolution->stack.end_time) {
            ctx.registry.remove<component::resolution>(entity);
        }
        if (aegis && ctx.current_tick > aegis->stack.end_time) {
            ctx.registry.remove<component::aegis>(entity);
        }

        // Conditions
        auto vulnerability = ctx.registry.try_get<component::vulnerability>(entity);
        auto burning = ctx.registry.try_get<component::burning>(entity);

        if (vulnerability) {
            while (!vulnerability->stacks.empty() &&
                   ctx.current_tick > vulnerability->stacks.top().end_time) {
                vulnerability->stacks.pop();
            }
            if (vulnerability->stacks.empty()) {
                ctx.registry.remove<component::vulnerability>(entity);
            }
        }
        if (burning) {
            while (!burning->stacks.empty() && ctx.current_tick > burning->stacks.top().end_time) {
                burning->stacks.pop();
            }
            if (burning->stacks.empty()) {
                ctx.registry.remove<component::burning>(entity);
            }
        }
    });
}

}  // namespace gw2combat::system
