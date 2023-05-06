#ifndef GW2COMBAT_AUDIT_REPORT_HPP
#define GW2COMBAT_AUDIT_REPORT_HPP

#include "common.hpp"

#include "tick_event.hpp"

#include "actor/attributes.hpp"
#include "actor/skill.hpp"

namespace gw2combat::audit {

struct report_t {
    std::vector<tick_event_t> tick_events;
    std::optional<std::string> error;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const report_t& nlohmann_json_t) {
    std::copy(nlohmann_json_t.tick_events.cbegin(),
              nlohmann_json_t.tick_events.cend(),
              std::back_inserter(nlohmann_json_j["tick_events"]));
    if (nlohmann_json_t.error) {
        nlohmann_json_j["error"] = *nlohmann_json_t.error;
    }
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, report_t& nlohmann_json_t) {
    report_t nlohmann_json_default_obj;
    nlohmann_json_t.tick_events =
        nlohmann_json_j.value("tick_events", nlohmann_json_default_obj.tick_events);
    if (nlohmann_json_j.contains("error")) {
        nlohmann_json_t.error = nlohmann_json_j.value("error", *nlohmann_json_default_obj.error);
    }
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_REPORT_HPP
