#ifndef GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP
#define GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP

#include "common.hpp"

namespace gw2combat::component {

struct bundle_component {
    std::string name;

    [[nodiscard]] inline bool operator==(const bundle_component& rhs) const {
        return name == rhs.name;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(bundle_component, name)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EQUIPMENT_BUNDLE_HPP