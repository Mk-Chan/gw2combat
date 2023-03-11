#ifndef GW2COMBAT_SYSTEM_AUDIT_HPP
#define GW2COMBAT_SYSTEM_AUDIT_HPP

#include "common.hpp"

#include "audit/report.hpp"

namespace gw2combat::system {

extern void audit(registry_t& registry);
extern audit::report_t get_audit_report(registry_t& registry);

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_AUDIT_HPP
