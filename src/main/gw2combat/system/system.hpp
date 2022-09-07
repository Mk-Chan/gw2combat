#ifndef GW2COMBAT_SYSTEM_HPP
#define GW2COMBAT_SYSTEM_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::system {

extern void init_combat_stats(registry_t& registry);
extern void progress_recharges(registry_t& registry);
extern void progress_cooldowns(registry_t& registry);
extern void progress_durations(registry_t& registry);
extern void expire_non_damaging_effects_with_no_duration(registry_t& registry);
extern void expire_damaging_effects_with_no_duration(registry_t& entity);
extern void calculate_effective_attributes(registry_t& registry);
extern void calculate_outgoing_damage_modifiers(registry_t& source_entity);
extern void calculate_incoming_damage_modifiers(registry_t& registry);
extern void continue_rotation(registry_t& registry, bool enable_checks = false);
extern void check_if_animation_finished(registry_t& registry);
extern void progress_animations(registry_t& registry);
extern void remove_animation_if_finished(registry_t& registry);
extern void do_skill(registry_t& registry);
extern void dispatch_strikes(registry_t& registry);
extern void dispatch_effects(registry_t& registry);
extern void apply_incoming_effects(registry_t& target_entity);
extern void apply_incoming_strikes(registry_t& registry);
extern void buffer_condition_damage(registry_t& registry,
                                    std::optional<entity_t> specific_effect_entity = std::nullopt);
extern void apply_condition_damage(registry_t& target_entity);
extern void destroy_after_rotation(registry_t& registry);
extern void update_combat_stats(registry_t& registry);
extern void reset_combat_stats(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_HPP
