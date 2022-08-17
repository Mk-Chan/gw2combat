#ifndef GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFICATIONS_HPP
#define GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFICATIONS_HPP

#include "gw2combat/actor/attributes.hpp"

namespace gw2combat::component {

struct attribute_modification_t {
    actor::attribute_t attribute;
    double multiplier = 1.0;
    double addend = 0.0;
};

// Applied on traits, effects or skills along with owner_actor
struct attribute_modifications_component {
    std::vector<attribute_modification_t> attribute_modifications;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modification_t,
                                                attribute,
                                                multiplier,
                                                addend)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFICATIONS_HPP
