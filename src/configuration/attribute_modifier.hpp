#ifndef GW2COMBAT_CONFIGURATION_ATTRIBUTE_MODIFIER_HPP
#define GW2COMBAT_CONFIGURATION_ATTRIBUTE_MODIFIER_HPP

#include "actor/attributes.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct attribute_modifier_t {
    condition_t condition{};

    actor::attribute_t attribute = actor::attribute_t::INVALID;
    double multiplier = 1.0;
    double addend = 0.0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modifier_t,
                                                condition,
                                                attribute,
                                                multiplier,
                                                addend)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ATTRIBUTE_MODIFIER_HPP
