#include "system.hpp"

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
#include "gw2combat/component/skills/successful_skill_cast.hpp"

namespace gw2combat::system {

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
                       component::pulse_conditions>();
}

void run_systems(system::context& ctx) {
    clear_temporary_components(ctx);

    if (ctx.current_tick % component::pulse_conditions::pulse_rate == 0) {
        ctx.registry.emplace<component::pulse_conditions>(*singleton_entity);
    }

    system::combat_detection(ctx);

    system::character_command(ctx);
    system::update_animation_state(ctx);

    system::effective_attributes_calculation(ctx);

    system::incoming_strike_damage_multiplier_calculation(ctx);
    system::incoming_condition_damage_multiplier_calculation(ctx);
    system::outgoing_condition_damage_multiplier_calculation(ctx);
    system::outgoing_strike_damage_multiplier_calculation(ctx);

    system::perform_instant_cast_skills(ctx);
    system::perform_successful_skill_cast_after_cast_effects(ctx);
    system::shield_of_wrath(ctx);
    system::sword_of_justice(ctx);

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

}  // namespace gw2combat::system
