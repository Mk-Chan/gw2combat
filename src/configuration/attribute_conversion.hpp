#ifndef GW2COMBAT_CONFIGURATION_ATTRIBUTE_CONVERSION_HPP
#define GW2COMBAT_CONFIGURATION_ATTRIBUTE_CONVERSION_HPP

#include "actor/attributes.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct attribute_conversion_t {
    condition_t condition{};

    actor::attribute_t from = actor::attribute_t::INVALID;
    actor::attribute_t to = actor::attribute_t::INVALID;
    double multiplier = 0.0;
    double addend = 0.0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_conversion_t,
                                                condition,
                                                from,
                                                to,
                                                multiplier,
                                                addend)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ATTRIBUTE_CONVERSION_HPP
