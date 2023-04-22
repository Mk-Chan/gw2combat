#ifndef GW2COMBAT_AUDIT_REPORT_HPP
#define GW2COMBAT_AUDIT_REPORT_HPP

#include "common.hpp"

#include "old_events.hpp"
#include "tick_event.hpp"

#include "actor/attributes.hpp"
#include "actor/skill.hpp"

namespace gw2combat::audit {

struct report_t {
    std::vector<std::variant<old_actor_created_event_t,
                             old_skill_cast_begin_event_t,
                             old_skill_cast_end_event_t,
                             old_equipped_bundle_event_t,
                             old_dropped_bundle_event_t,
                             old_effect_application_event_t,
                             old_damage_event_t,
                             old_combat_stats_update_event_t,
                             old_effect_expired_event_t,
                             old_actor_downstate_event_t>>
        events;
    std::vector<tick_event_t> tick_events;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const report_t& nlohmann_json_t) {
    std::for_each(
        nlohmann_json_t.events.begin(), nlohmann_json_t.events.end(), [&](const auto& event) {
            std::visit([&](auto&& e) { nlohmann_json_j["events"].emplace_back(e); }, event);
        });
    std::copy(nlohmann_json_t.tick_events.cbegin(),
              nlohmann_json_t.tick_events.cend(),
              std::back_inserter(nlohmann_json_j["tick_events"]));
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
    nlohmann_json_t.tick_events =
        nlohmann_json_j.value("tick_events", nlohmann_json_default_obj.tick_events);
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_REPORT_HPP
