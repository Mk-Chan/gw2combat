#include <spdlog/spdlog.h>

#include "system.hpp"

#include "gw2combat/component/is_character.hpp"
#include "gw2combat/component/profession/guardian/virtue_of_justice.hpp"
#include "gw2combat/component/successfully_cast_skill.hpp"

namespace gw2combat::system {

void perform_animation(context& ctx) {
    ctx.registry.view<component::is_character>().each([&](const entt::entity entity) {
        auto successfully_cast_skill_ptr =
            ctx.registry.try_get<component::successfully_cast_skill>(entity);
        if (successfully_cast_skill_ptr) {
            auto& virtue_of_justice = ctx.registry.get<component::virtue_of_justice>(entity);
            // Maybe += number of targets hit? Well, for now only single target is supported
            // every 3 hits because of permeating wrath
            virtue_of_justice.consecutive_successful_hits =
                (virtue_of_justice.consecutive_successful_hits % 3) + 1;
            if (virtue_of_justice.consecutive_successful_hits == 3) {
                virtue_of_justice.apply_burning_on_tick = ctx.current_tick;
            }
        }
    });
}

}  // namespace gw2combat::system
