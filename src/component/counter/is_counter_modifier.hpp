#ifndef GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_MODIFIER_HPP
#define GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_MODIFIER_HPP

#include "configuration/counter_modifier.hpp"

namespace gw2combat::component {

struct is_counter_modifier_t {
    std::vector<configuration::counter_modifier_t> counter_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(is_counter_modifier_t, counter_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COUNTER_IS_COUNTER_MODIFIER_HPP
