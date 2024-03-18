#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include "configuration/audit.hpp"

#include "audit/tick_event.hpp"

namespace gw2combat::component {

struct audit_component {
    configuration::audit_t audit_configuration;
    std::vector<audit::tick_event_t> events;
    std::map<std::string, int> afk_ticks_by_actor;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
