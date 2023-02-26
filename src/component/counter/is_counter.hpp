#ifndef GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_HPP
#define GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_HPP

#include "configuration/counter_configuration.hpp"

namespace gw2combat::component {

struct is_counter {
    int value = 0;
    configuration::counter_configuration_t counter_configuration;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(is_counter, value, counter_configuration)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_HPP
