#include "system.hpp"

#include "gw2combat/component/character/dynamic_attributes.hpp"
#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/static_attributes.hpp"
#include "gw2combat/component/character/targeting.hpp"
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
#include "gw2combat/system/staged/profession_systems.hpp"
#include "gw2combat/system/staged/skill_systems.hpp"
#include "gw2combat/system/staged/trait_systems.hpp"

namespace gw2combat::system {

void clear_temporary_components(registry_t& registry) {
    registry.clear<component::effective_attributes,
                   component::incoming_strike_damage_multiplier,
                   component::incoming_condition_damage_multiplier,
                   component::outgoing_strike_damage_multiplier,
                   component::outgoing_condition_damage_multiplier,

                   component::outgoing_strike_damage,
                   component::incoming_strike_damage,

                   component::outgoing_condition_application,
                   component::incoming_condition_application,

                   component::effective_incoming_damage,
                   component::instant_cast_skills>();
    registry.ctx().erase<component::pulse_conditions>();
}

template <combat_stage stage>
void run_staged_systems(registry_t& registry, tick_t current_tick) {
    system::virtue_of_justice<stage>(registry, current_tick);
    system::inspiring_virtue<stage>(registry, current_tick);

    system::symbolic_avenger<stage>(registry, current_tick);
    system::symbolic_power<stage>(registry, current_tick);

    system::on_hit_effect_applications<stage>(registry, current_tick);
}

void run_systems(registry_t& registry, tick_t current_tick) {
    clear_temporary_components(registry);

    if (current_tick % component::pulse_conditions::pulse_rate == 0) {
        registry.ctx().emplace<component::pulse_conditions>();
    }

    system::combat_detection(registry, current_tick);

    system::effective_attributes_calculation(registry, current_tick);
    system::incoming_strike_damage_multiplier_calculation(registry, current_tick);
    system::incoming_condition_damage_multiplier_calculation(registry, current_tick);
    system::outgoing_condition_damage_multiplier_calculation(registry, current_tick);
    system::outgoing_strike_damage_multiplier_calculation(registry, current_tick);

    system::expire_non_damaging_effects(registry, current_tick);
    system::expire_damaging_effects(registry, current_tick);

    system::character_command(registry, current_tick);
    system::cast_skills(registry, current_tick);

    run_staged_systems<combat_stage::BEFORE_OUTGOING_STRIKE_BUFFERING>(registry, current_tick);

    system::incoming_strike_detection(registry, current_tick);

    run_staged_systems<combat_stage::AFTER_OUTGOING_STRIKE_BUFFERING>(registry, current_tick);
    run_staged_systems<combat_stage::BEFORE_INCOMING_STRIKE_DAMAGE_CALCULATION>(registry,
                                                                                current_tick);

    system::incoming_strike_damage_calculation(registry, current_tick);

    run_staged_systems<combat_stage::AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION>(registry,
                                                                               current_tick);

    system::incoming_condition_detection(registry, current_tick);
    system::incoming_condition_application(registry, current_tick);
    system::pulse_conditions(registry, current_tick);

    system::update_health(registry, current_tick);
    system::downstate_detection(registry, current_tick);
    system::destroy_after_rotation_entities(registry, current_tick);
}

}  // namespace gw2combat::system
