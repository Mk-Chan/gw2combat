#include "system.hpp"

#include <spdlog/spdlog.h>

#include "gw2combat/component/pulse_condition.hpp"

namespace gw2combat::system {

void check_if_is_condition_pulse_tick(context& ctx) {
    if (ctx.current_tick % 1'000 == 0) {
        ctx.registry.emplace<component::pulse_condition>(*singleton_entity);
        spdlog::info("tick: {}, pulsing condition damage", ctx.current_tick);
    }
}

}  // namespace gw2combat::system
