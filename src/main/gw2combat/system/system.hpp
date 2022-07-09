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
extern void perform_animation(context&);
extern void character_command(context&);
extern void effect_expiration(context&);
extern void outgoing_strike_damage_calculation(context&);
extern void incoming_strike_damage_detection(context& ctx);
extern void incoming_strike_damage_calculation(context& ctx);
extern void update_combat_stats(context& ctx);
extern void downstate_detection(context& ctx);
extern void accumulate_skill_cast_ticks(context& ctx);
extern void check_if_is_condition_pulse_tick(context& is_condition_pulse);
extern void incoming_condition_damage_calculation(context& ctx);
extern void outgoing_condition_application(context& ctx);
extern void update_animation_lock_state(context& ctx);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_SYSTEM_HPP
