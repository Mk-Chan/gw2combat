#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include <variant>

#include "audit/events.hpp"

namespace gw2combat::component {

struct audit_component {
    std::vector<std::variant<audit::damage_event_t,
                             audit::skill_cast_begin_event_t,
                             audit::skill_cast_end_event_t,
                             audit::effect_application_event_t>>
        events;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
