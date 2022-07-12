#ifndef GW2COMBAT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_HPP

#include <algorithm>

namespace gw2combat::utils {

static inline int critical_chance_pct_to_precision(double critical_chance_pct) {
    return critical_chance_pct * 21.0 + 895.0;
}

static inline double calculate_critical_chance_multiplier(unsigned int precision,
                                                          unsigned int ferocity) {
    double critical_chance = 0.05 + ((double)precision - 1000.0) / 2100.0;
    double critical_damage = 1.5 + (double)ferocity / 1500.0;
    return 1.0 + std::min(critical_chance, 1.0) * (critical_damage - 1.0);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_HPP
