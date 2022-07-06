#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/condition_tick_status.hpp"

namespace gw2combat::system {

void process_condition_tick(context& ctx) {
    ctx.registry.view<component::condition_tick_status>().each(
        [&](component::condition_tick_status& condition_tick_status) {
            condition_tick_status.apply_condition_damage = ctx.current_tick % 1'000 == 0;
            if (condition_tick_status.apply_condition_damage) {
                spdlog::info("tick: {}, apply_condition_damage: {}",
                             ctx.current_tick,
                             condition_tick_status.apply_condition_damage);
            }
        });
}

}  // namespace gw2combat::system
