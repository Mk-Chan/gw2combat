#ifndef GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP
#define GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP

#include "common.hpp"

#include "actor/counter.hpp"
#include "condition.hpp"

namespace gw2combat::configuration {

struct counter_configuration_t {
    actor::counter_t counter_key;
    int initial_value = 0;
    int reset_at_value = 1;
    std::vector<condition_t> increment_conditions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(counter_configuration_t,
                                                counter_key,
                                                initial_value,
                                                reset_at_value,
                                                increment_conditions)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP
