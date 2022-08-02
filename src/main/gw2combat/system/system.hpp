#ifndef GW2COMBAT_SYSTEM_SYSTEM_HPP
#define GW2COMBAT_SYSTEM_SYSTEM_HPP

#include "gw2combat/types.hpp"
#include "gw2combat/utilities.hpp"

namespace gw2combat::system {

extern void run_systems(registry_t& registry, tick_t current_tick);

extern void combat_detection(registry_t& registry, tick_t current_tick);

extern void character_command(registry_t& registry, tick_t current_tick);
extern void cast_skills(registry_t& registry, tick_t current_tick);

extern void incoming_condition_application(registry_t& registry, tick_t current_tick);
extern void incoming_strike_detection(registry_t& registry, tick_t current_tick);
extern void incoming_condition_detection(registry_t& registry, tick_t current_tick);

extern void incoming_strike_damage_multiplier_calculation(registry_t& registry,
                                                          tick_t current_tick);
extern void incoming_condition_damage_multiplier_calculation(registry_t& registry,
                                                             tick_t current_tick);
extern void outgoing_condition_damage_multiplier_calculation(registry_t& registry,
                                                             tick_t current_tick);
extern void outgoing_strike_damage_multiplier_calculation(registry_t& registry,
                                                          tick_t current_tick);

extern void incoming_strike_damage_calculation(registry_t& registry, tick_t current_tick);
extern void pulse_conditions(registry_t& registry, tick_t current_tick);

extern void expire_damaging_effects(registry_t& registry, tick_t current_tick);
extern void expire_non_damaging_effects(registry_t& registry, tick_t current_tick);

extern void effective_attributes_calculation(registry_t& registry, tick_t current_tick);
extern void update_health(registry_t& registry, tick_t current_tick);
extern void downstate_detection(registry_t& registry, tick_t current_tick);

extern void destroy_after_rotation_entities(registry_t& registry, tick_t current_tick);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_SYSTEM_HPP
