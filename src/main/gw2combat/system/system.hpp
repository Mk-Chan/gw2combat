#ifndef GW2COMBAT_SYSTEM_SYSTEM_HPP
#define GW2COMBAT_SYSTEM_SYSTEM_HPP

#include <entt/entt.hpp>

#include "gw2combat/entity.hpp"
#include "gw2combat/types.hpp"

#include "gw2combat/rotation.hpp"

namespace gw2combat::system {

struct context {
    tick_t current_tick;
    tick_t tick_rate;
    entt::registry& registry;
    std::optional<predetermined_rotation> predetermined_rotation = std::nullopt;
};

extern void incoming_condition_application(context& ctx);
extern void incoming_strike_detection(context& ctx);

extern void incoming_strike_damage_multiplier_calculation(context& ctx);
extern void incoming_condition_damage_multiplier_calculation(context& ctx);
extern void outgoing_condition_damage_multiplier_calculation(context& ctx);
extern void outgoing_strike_damage_multiplier_calculation(context& ctx);

extern void calculate_outgoing_strike_damage_for_channeling_skill_no_after_cast(context& ctx);
extern void calculate_outgoing_strike_damage_for_casting_skill_no_after_cast(context& ctx);

extern void incoming_strike_damage_calculation(context& ctx);
extern void pulse_conditions(context& ctx);

extern void virtue_of_justice(context& ctx);
extern void shield_of_wrath(context& ctx);

extern void expire_damaging_effects(context& ctx);
extern void expire_non_damaging_effects(context& ctx);

extern void combat_detection(context& ctx);
extern void effective_attributes_calculation(context& ctx);
extern void character_command(context& ctx);
extern void perform_instant_cast_skills(context& ctx);
extern void update_health(context& ctx);
extern void downstate_detection(context& ctx);
extern void update_animation_state(context& ctx);

extern void run_systems(system::context& ctx);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_SYSTEM_HPP
