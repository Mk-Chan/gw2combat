#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include "component/damage/incoming_damage.hpp"

namespace gw2combat::component {

struct audit_component {
    std::string audit_base_path;
    std::vector<component::incoming_damage_event> incoming_damage_events;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
