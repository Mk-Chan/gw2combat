#ifndef GW2COMBAT_COMPONENT_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP

#include "filters.hpp"

#include "gw2combat/actor/attributes.hpp"

namespace gw2combat::component {

struct attribute_conversions_t {
    filters_t filters;
    actor::attribute_t from;
    actor::attribute_t to;
    double multiplier;
    double addend;
};

// This component is applied on skills/traits/effects with owner_actor
struct attribute_conversions_component {
    std::vector<attribute_conversions_t> attribute_conversions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_conversions_t,
                                                filters,
                                                from,
                                                to,
                                                multiplier,
                                                addend)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_conversions_component,
                                                attribute_conversions)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ATTRIBUTE_CONVERSIONS_COMPONENT_HPP
