#ifndef GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline double get_critical_strike_multiplier(double critical_chance_pct,
                                                               double critical_damage_pct) {
    if constexpr (DETERMINISTIC_SIMULATION) {
        return (1.0 + (std::min(critical_chance_pct, 100.0) / 100.0) *
                          (critical_damage_pct / 100.0 - 1.0));
    } else {
        return utils::check_random_success(critical_chance_pct) ? critical_damage_pct / 100.0 : 1.0;
    }
}

[[nodiscard]] static inline double get_critical_strike_multiplier(
    const component::effective_attributes& effective_attributes) {
    if constexpr (DETERMINISTIC_SIMULATION) {
        return (1.0 +
                (std::min(effective_attributes[actor::attribute_t::CRITICAL_CHANCE_PCT], 100.0) /
                 100.0) *
                    (effective_attributes[actor::attribute_t::CRITICAL_DAMAGE_PCT] / 100.0 - 1.0));
    } else {
        return utils::check_random_success(
                   effective_attributes[actor::attribute_t::CRITICAL_CHANCE_PCT])
                   ? effective_attributes[actor::attribute_t::CRITICAL_DAMAGE_PCT] / 100.0
                   : 1.0;
    }
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_DAMAGE_UTILITIES_HPP
