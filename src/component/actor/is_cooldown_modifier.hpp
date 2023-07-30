#ifndef GW2COMBAT_COMPONENT_COUNTER_IS_COOLDOWN_MODIFIER_HPP
#define GW2COMBAT_COMPONENT_COUNTER_IS_COOLDOWN_MODIFIER_HPP

#include "configuration/cooldown_modifier.hpp"

namespace gw2combat::component {

struct is_cooldown_modifier_t {
    std::vector<configuration::cooldown_modifier_t> cooldown_modifiers{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(is_cooldown_modifier_t, cooldown_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_COUNTER_IS_COOLDOWN_MODIFIER_HPP
