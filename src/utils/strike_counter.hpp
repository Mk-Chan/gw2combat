#ifndef GW2COMBAT_UTILS_STRIKE_COUNTER_HPP
#define GW2COMBAT_UTILS_STRIKE_COUNTER_HPP

#include "common.hpp"

namespace gw2combat::utils {

struct strike_counter_t {
    int start = 0;
    int current = 0;
    int required_count = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_counter_t, start, current, required_count)

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_STRIKE_COUNTER_HPP
