#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include "audit/events.hpp"

namespace gw2combat::component {

struct audit_component {
    std::vector<std::variant<audit::actor_created_event_t,
                             audit::skill_cast_begin_event_t,
                             audit::skill_cast_end_event_t,
                             audit::equipped_bundle_event_t,
                             audit::dropped_bundle_event_t,
                             audit::effect_application_event_t,
                             audit::damage_event_t,
                             audit::combat_stats_update_event_t,
                             audit::effect_expired_event_t,
                             audit::actor_downstate_event_t>>
        events;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
