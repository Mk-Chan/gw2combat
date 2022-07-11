#include "game_loop.hpp"

#include <spdlog/spdlog.h>

#include "entity.hpp"

#include "system/system.hpp"

#include "component/outgoing_strike_damage.hpp"
#include "gw2combat/component/downstate.hpp"
#include "gw2combat/component/effective_attributes.hpp"
#include "gw2combat/component/effective_incoming_damage.hpp"
#include "gw2combat/component/incoming_strike_damage.hpp"
#include "gw2combat/component/outgoing_condition_application.hpp"
#include "gw2combat/component/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/pulse_condition.hpp"
#include "gw2combat/component/successful_skill_cast.hpp"

namespace gw2combat {

void clear_temporary_components(system::context& ctx) {
    ctx.registry.clear<component::outgoing_strike_damage_multiplier,
                       component::outgoing_condition_damage_multiplier,
                       component::outgoing_strike_damage,
                       component::outgoing_condition_application,
                       component::incoming_strike_damage,
                       component::effective_incoming_damage,
                       component::effective_attributes,
                       component::successful_skill_cast,
                       component::pulse_condition>();
}

void run_systems(system::context& ctx) {
    clear_temporary_components(ctx);

    system::check_effect_expirations(ctx);
    system::check_if_is_condition_pulse_tick(ctx);
    system::combat_detection(ctx);
    system::update_animation_state(ctx);
    system::character_command(ctx);
    system::effective_attributes_calculation(ctx);
    system::outgoing_condition_damage_multiplier_calculation(ctx);
    system::outgoing_strike_damage_multiplier_calculation(ctx);
    system::calculate_outgoing_strike_damage_for_channeling_skill_no_after_cast(ctx);
    system::calculate_outgoing_strike_damage_for_casting_skill_no_after_cast(ctx);
    system::virtue_of_justice(ctx);
    system::incoming_condition_application(ctx);
    system::incoming_strike_detection(ctx);
    system::burning_damage_calculation(ctx);
    system::strike_damage_calculation(ctx);
    system::update_health(ctx);
    system::expire_effects(ctx);
    system::downstate_detection(ctx);
}

void game_loop() {
    entt::registry registry;

    init_entities(registry);
    system::context ctx{tick_t{0}, tick_t{1}, registry};

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
