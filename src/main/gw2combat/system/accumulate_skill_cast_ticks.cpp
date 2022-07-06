#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/animation.hpp"
#include "gw2combat/component/boon/quickness.hpp"

namespace gw2combat::system {

void accumulate_skill_cast_ticks(context& ctx) {
    ctx.registry.view<component::animation>().each([&](const entt::entity entity,
                                                       component::animation& animation) {
        tick_t effective_tick_rate = ctx.tick_rate;
        if (ctx.registry.any_of<component::quickness>(entity)) {
            effective_tick_rate = int(double(effective_tick_rate) * 1.5);
        }
        animation.accumulated_ticks += effective_tick_rate;

        //spdlog::info("entity: {}, effective_tick_rate: {}, accumulated_ticks: {}, need_ticks: {}",
        //             static_cast<std::uint32_t>(entity),
        //             effective_tick_rate,
        //             animation.accumulated_ticks,
        //             animation.required_ticks_for_completion);
    });
}

}  // namespace gw2combat::system
