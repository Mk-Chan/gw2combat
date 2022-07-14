#include <catch2/catch_test_macros.hpp>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include "gw2combat/entity.hpp"
#include "gw2combat/system/system.hpp"
#include "gw2combat/types.hpp"

#include "gw2combat/component/character/downstate.hpp"
#include "gw2combat/component/no_more_rotation.hpp"

namespace gw2combat {

TEST_CASE("4Mil Golem AA-only Benchmark", "[game_loop]") {
    entt::registry registry;
    predetermined_rotation rotation = read_rotation("src/test/resources/rotation.csv");

    init_entities(registry);
    system::context ctx{tick_t{0}, tick_t{1}, registry, std::make_optional(rotation)};

    while (!ctx.registry.any_of<component::no_more_rotation>(*singleton_entity)) {
        run_systems(ctx);

        for (auto&& [entity] : ctx.registry.view<component::downstate>().each()) {
            spdlog::info("tick: {}, entity: {} is downstate!",
                         ctx.current_tick,
                         static_cast<std::uint32_t>(entity));
            goto loop_end;
        }

        ctx.current_tick += ctx.tick_rate;
    }
loop_end:
    REQUIRE(ctx.registry.any_of<component::downstate>(entt::entity{2}));
}

}  // namespace gw2combat
