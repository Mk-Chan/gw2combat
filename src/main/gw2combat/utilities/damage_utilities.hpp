#ifndef GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline double get_critical_hit_multiplier(
    const component::effective_attributes& effective_attributes) {
    return (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                      (effective_attributes.critical_damage_pct / 100.0 - 1.0));
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP
