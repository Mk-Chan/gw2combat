#ifndef GW2COMBAT_SYSTEM_SYSTEM_HPP
#define GW2COMBAT_SYSTEM_SYSTEM_HPP

#include <entt/entt.hpp>

#include "gw2combat/entity.hpp"
#include "gw2combat/types.hpp"

namespace gw2combat::system {

struct context {
    tick_t current_tick;
    tick_t tick_rate;
    entt::registry& registry;
};

extern void combat_detection(context&);
extern void effective_attributes_calculation(context&);
extern void character_command(context&);
extern void effect_expiration(context&);
extern void virtue_of_justice(context&);
extern void incoming_condition_application(context&);
extern void outgoing_strike_calculations(context&);
extern void incoming_strike_detection(context&);
extern void strike_damage_calculation(context&);
extern void update_health(context&);
extern void downstate_detection(context&);
extern void accumulate_skill_cast_ticks(context&);
extern void check_if_is_condition_pulse_tick(context&);
extern void burning_damage_calculation(context&);
extern void update_animation_lock_state(context&);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_SYSTEM_HPP
