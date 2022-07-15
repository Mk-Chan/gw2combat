#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/skills/guardian/sword_of_justice.hpp"
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
                ctx.registry.emplace<component::sword_of_justice>(
                    sword_of_justice_entity, component::sword_of_justice{entity, ctx.current_tick});
                spdlog::info("tick: {}, created sword of justice", ctx.current_tick);
            }
        });
}

}  // namespace gw2combat::system
