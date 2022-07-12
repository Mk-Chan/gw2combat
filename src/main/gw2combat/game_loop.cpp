#include "game_loop.hpp"

#include <spdlog/spdlog.h>
#include <filesystem>

#include "entity.hpp"

#include "system/system.hpp"

#include "gw2combat/component/character/downstate.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/damage/effective_incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_strike_damage.hpp"
#include "gw2combat/component/damage/multipliers/incoming_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/incoming_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/damage/pulse_conditions.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"
#include "gw2combat/component/skills/successful_skill_cast.hpp"

namespace gw2combat {

void clear_temporary_components(system::context& ctx) {
    ctx.registry.clear<component::effective_attributes,

                       component::incoming_strike_damage_multiplier,
                       component::incoming_condition_damage_multiplier,
                       component::outgoing_strike_damage_multiplier,
                       component::outgoing_condition_damage_multiplier,

                       component::outgoing_strike_damage,
                       component::outgoing_condition_application,

                       component::incoming_strike_damage,
                       component::effective_incoming_damage,

                       component::successful_skill_cast,
                       component::pulse_conditions,
                       component::instant_cast_skills>();
}

void run_systems(system::context& ctx) {
    clear_temporary_components(ctx);

    system::combat_detection(ctx);
    system::update_animation_state(ctx);

    system::effective_attributes_calculation(ctx);

    system::incoming_strike_damage_multiplier_calculation(ctx);
    system::incoming_condition_damage_multiplier_calculation(ctx);
    system::outgoing_condition_damage_multiplier_calculation(ctx);
    system::outgoing_strike_damage_multiplier_calculation(ctx);

    system::character_command(ctx);
    // system::perform_instant_cast_skills(ctx);
    // system::shield_of_wrath(ctx);

    system::calculate_outgoing_strike_damage_for_channeling_skill_no_after_cast(ctx);
    system::calculate_outgoing_strike_damage_for_casting_skill_no_after_cast(ctx);

    system::incoming_strike_detection(ctx);

    system::virtue_of_justice(ctx);

    system::incoming_condition_application(ctx);

    system::expire_damaging_effects(ctx);
    system::pulse_conditions(ctx);
    system::incoming_strike_damage_calculation(ctx);

    system::update_health(ctx);
    system::expire_non_damaging_effects(ctx);
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
