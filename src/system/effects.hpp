#ifndef GW2COMBAT_SYSTEM_EFFECTS_HPP
#define GW2COMBAT_SYSTEM_EFFECTS_HPP

#include "common.hpp"

namespace gw2combat::system {

extern void buffer_damage_for_effects_with_no_duration(registry_t& registry);
extern void apply_condition_damage(registry_t& registry);
extern void buffer_condition_damage(registry_t& registry,
                                    std::optional<entity_t> specific_effect_entity = std::nullopt);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_EFFECTS_HPP
