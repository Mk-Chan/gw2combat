#include "game_loop.hpp"

#include <spdlog/spdlog.h>

#include "entity.hpp"

#include "system/system.hpp"

#include "component/outgoing_damage.hpp"
#include "gw2combat/component/downstate.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"
#include "gw2combat/component/incoming_damage.hpp"
#include "gw2combat/component/pulse_condition.hpp"
#include "gw2combat/component/successfully_cast_skill.hpp"

namespace gw2combat {

void clear_temporary_components(system::context& ctx) {
    ctx.registry.clear<component::outgoing_damage,
                       component::incoming_damage,
                       component::effective_incoming_damage,
                       component::effective_attributes,
                       component::successfully_cast_skill,
                       component::pulse_condition>();
}

void run_systems(system::context& ctx) {
    clear_temporary_components(ctx);

    system::combat_detection(ctx);
    system::check_if_is_condition_pulse_tick(ctx);
    system::update_animation_lock_state(ctx);
    system::character_command(ctx);
    system::accumulate_skill_cast_ticks(ctx);
    system::perform_animation(ctx);
    system::effect_expiration(ctx);
    system::effective_attributes_calculation(ctx);
    system::outgoing_strike_damage_calculation(ctx);
    system::outgoing_condition_application(ctx);
    system::incoming_strike_damage_detection(ctx);
    system::incoming_strike_damage_calculation(ctx);
    system::incoming_condition_damage_calculation(ctx);
    system::update_combat_stats(ctx);
    system::downstate_detection(ctx);
}

void game_loop() {
    entt::registry registry;
    init_entities(registry);
    system::context ctx{tick_t{0}, tick_t{10}, registry};

    while (true) {
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
    return;
}

}  // namespace gw2combat
