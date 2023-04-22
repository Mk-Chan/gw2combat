#ifndef GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP

#include "common.hpp"

#include "configuration/audit.hpp"

#include "audit/old_events.hpp"
#include "audit/tick_event.hpp"

namespace gw2combat::component {

struct audit_component {
    configuration::audit_t audit_configuration;
    std::vector<std::variant<audit::old_actor_created_event_t,
                             audit::old_skill_cast_begin_event_t,
                             audit::old_skill_cast_end_event_t,
                             audit::old_equipped_bundle_event_t,
                             audit::old_dropped_bundle_event_t,
                             audit::old_effect_application_event_t,
                             audit::old_damage_event_t,
                             audit::old_combat_stats_update_event_t,
                             audit::old_effect_expired_event_t,
                             audit::old_actor_downstate_event_t>>
        old_events;
    std::vector<audit::tick_event_t> events;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_AUDIT_COMPONENT_HPP
