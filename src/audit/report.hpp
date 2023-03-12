#ifndef GW2COMBAT_AUDIT_REPORT_HPP
#define GW2COMBAT_AUDIT_REPORT_HPP

#include "common.hpp"

#include "events.hpp"

namespace gw2combat::audit {

struct report_t {
    std::vector<std::variant<actor_created_event_t,
                             skill_cast_begin_event_t,
                             skill_cast_end_event_t,
                             equipped_bundle_event_t,
                             dropped_bundle_event_t,
                             effect_application_event_t,
                             damage_event_t,
                             combat_stats_update_event_t,
                             effect_expired_event_t,
                             actor_downstate_event_t>>
        events;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const report_t& nlohmann_json_t) {
    std::for_each(
        nlohmann_json_t.events.begin(), nlohmann_json_t.events.end(), [&](const auto& event) {
            std::visit([&](auto&& e) { nlohmann_json_j["events"].emplace_back(e); }, event);
        });
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, report_t& nlohmann_json_t) {
    report_t nlohmann_json_default_obj;
    decltype(report_t::events) const* events_ptr;
    if (nlohmann_json_j.contains("events")) {
        events_ptr = (decltype(report_t::events) const*)(&nlohmann_json_j.at("events"));
    } else {
        events_ptr = &nlohmann_json_default_obj.events;
    }
    std::copy(events_ptr->cbegin(), events_ptr->cend(), std::back_inserter(nlohmann_json_t.events));
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_REPORT_HPP
