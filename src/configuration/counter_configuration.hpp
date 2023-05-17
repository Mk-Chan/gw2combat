#ifndef GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP
#define GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP

#include "common.hpp"

#include "actor/counter.hpp"
#include "condition.hpp"
#include "counter_modifier.hpp"

namespace gw2combat::configuration {

struct counter_configuration_t {
    actor::counter_t counter_key;
    int initial_value = 0;
    std::vector<counter_modifier_t> counter_modifiers{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(counter_configuration_t,
                                                counter_key,
                                                initial_value,
                                                counter_modifiers)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_COUNTER_CONFIGURATION_HPP
