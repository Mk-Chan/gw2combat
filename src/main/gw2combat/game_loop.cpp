#include "game_loop.hpp"

#include <spdlog/spdlog.h>

#include "entity.hpp"
#include "rotation.hpp"

#include "system/system.hpp"

#include "gw2combat/component/animation/animation.hpp"
#include "gw2combat/component/character/downstate.hpp"
#include "gw2combat/component/no_more_rotation.hpp"

namespace gw2combat {

void game_loop() {
    entt::registry registry;
    predetermined_rotation rotation = read_rotation("src/test/resources/rotation.csv");

    init_entities(registry);
    system::context ctx{tick_t{0}, tick_t{1}, registry, std::make_optional(rotation)};

    bool reported_downstate = false;
    while (true) {
        run_systems(ctx);

        for (auto&& [entity] : ctx.registry.view<component::downstate>().each()) {
            spdlog::info("tick: {}, entity: {} is downstate!",
                         ctx.current_tick,
                         static_cast<std::uint32_t>(entity));
            reported_downstate = true;
        }
        if (reported_downstate) {
            break;
        }

        ctx.current_tick += ctx.tick_rate;
    }
    spdlog::info("tick: {}, done!", ctx.current_tick);
}

}  // namespace gw2combat
