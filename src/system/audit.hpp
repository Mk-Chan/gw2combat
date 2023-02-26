#ifndef GW2COMBAT_SYSTEM_AUDIT_HPP
#define GW2COMBAT_SYSTEM_AUDIT_HPP

#include "common.hpp"

namespace gw2combat::system {

struct audit_report {
    struct damage_event {
        std::string damage_source;
        tick_t time_ms;
        int damage;
    };
    std::vector<damage_event> damage_events;
    int remaining_health;
};

extern void audit_damage(registry_t& registry);

extern void audit_report_to_disk(registry_t& registry);
extern audit_report get_audit_report(registry_t& registry, bool exclude_console = true);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_report::damage_event,
                                                damage_source,
                                                time_ms,
                                                damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_report, damage_events, remaining_health)

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_AUDIT_HPP
