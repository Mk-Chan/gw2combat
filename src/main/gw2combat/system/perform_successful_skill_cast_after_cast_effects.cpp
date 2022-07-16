#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/skills/guardian/spirit_weapon.hpp"
#include "gw2combat/component/skills/guardian/symbol.hpp"
#include "gw2combat/component/skills/successful_skill_cast.hpp"

namespace gw2combat::system {

void perform_successful_skill_cast_after_cast_effects(context& ctx) {
    ctx.registry.view<component::successful_skill_cast>().each(
        [&](const entt::entity entity,
            const component::successful_skill_cast& successful_skill_cast) {
            auto& skill = successful_skill_cast.skill;
            if (skill.type != skills::skill::type::CASTING_WITH_AFTER_CAST) {
                return;
            }
            if (skill.name == "Binding Blade"_hs) {
                // TODO: Should actually be after detecting that an entity has been hit by binding
                //       blade
                auto& outgoing_condition_application =
                    ctx.registry.get_or_emplace<component::outgoing_condition_application>(entity);
                outgoing_condition_application.binding_blade_effect =
                    effect{entity, ctx.current_tick, 10'000};
                spdlog::info("tick: {}, outgoing binding blade", ctx.current_tick);
            } else if (skill.name == "Sword of Justice"_hs) {
                auto sword_of_justice_entity = ctx.registry.create();
                ctx.registry.emplace<component::spirit_weapon>(
                    sword_of_justice_entity,
                    component::spirit_weapon{entity,
                                             ctx.current_tick,
                                             skills::get_by_name("Sword of Justice Attack"_hs)});
                spdlog::info("tick: {}, created sword of justice", ctx.current_tick);
            } else if (skill.name == "Symbol of Resolution"_hs) {
                auto symbol_of_resolution_entity = ctx.registry.create();
                ctx.registry.emplace<component::symbol>(
                    symbol_of_resolution_entity,
                    component::symbol{entity,
                                      ctx.current_tick,
                                      skills::get_by_name("Symbol of Resolution Attack"_hs)});
                spdlog::info("tick: {}, created symbol of resolution", ctx.current_tick);
            }
        });
}

}  // namespace gw2combat::system
