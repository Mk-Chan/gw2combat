#ifndef GW2COMBAT_SYSTEM_AUDIT_HPP
#define GW2COMBAT_SYSTEM_AUDIT_HPP

#include "common.hpp"

#include "audit/report.hpp"

namespace gw2combat::system {

extern audit::tick_event_t create_tick_event(const decltype(audit::tick_event_t::event)& event,
                                             entity_t actor_entity,
                                             registry_t& registry);
extern void audit(registry_t& registry);
extern audit::report_t get_audit_report(registry_t& registry,
                                        int offset = 0,
                                        const std::string& error = {});

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_AUDIT_HPP
