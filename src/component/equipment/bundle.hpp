#ifndef GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP
#define GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP

#include "common.hpp"

#include "actor/bundle.hpp"

namespace gw2combat::component {

struct bundle_component {
    actor::bundle_t name;
};

struct equipped_bundle {};
struct dropped_bundle {
    actor::bundle_t name;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(bundle_component, name)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP
