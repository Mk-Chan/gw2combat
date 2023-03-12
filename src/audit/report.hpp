#ifndef GW2COMBAT_AUDIT_REPORT_HPP
#define GW2COMBAT_AUDIT_REPORT_HPP

#include "common.hpp"

#include "events.hpp"

namespace gw2combat::audit {

struct report_t {
    // NOTE: Don't forget to update to_json/from_json when adding more variants
    std::vector<std::variant<actor_created_event_t,
                             skill_cast_begin_event_t,
                             skill_cast_end_event_t,
                             effect_application_event_t,
                             damage_event_t,
                             combat_stats_update_event_t>>
        events;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const report_t& nlohmann_json_t) {
    std::for_each(
        nlohmann_json_t.events.begin(), nlohmann_json_t.events.end(), [&](const auto& event) {
            if (std::holds_alternative<actor_created_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(std::get<actor_created_event_t>(event));
            } else if (std::holds_alternative<skill_cast_begin_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(std::get<skill_cast_begin_event_t>(event));
            } else if (std::holds_alternative<skill_cast_end_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(std::get<skill_cast_end_event_t>(event));
            } else if (std::holds_alternative<effect_application_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(std::get<effect_application_event_t>(event));
            } else if (std::holds_alternative<damage_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(std::get<damage_event_t>(event));
            } else if (std::holds_alternative<combat_stats_update_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(
                    std::get<combat_stats_update_event_t>(event));
            }
        });
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, report_t& nlohmann_json_t) {
    report_t nlohmann_json_default_obj;
    std::vector<std::variant<actor_created_event_t,
                             skill_cast_begin_event_t,
                             skill_cast_end_event_t,
                             effect_application_event_t,
                             damage_event_t,
                             combat_stats_update_event_t>> const* events_ptr;
    if (nlohmann_json_j.contains("events")) {
        events_ptr =
            (std::vector<std::variant<actor_created_event_t,
                                      skill_cast_begin_event_t,
                                      skill_cast_end_event_t,
                                      effect_application_event_t,
                                      damage_event_t,
                                      combat_stats_update_event_t>> const*)(&nlohmann_json_j
                                                                                 .at("events"));
    } else {
        events_ptr = &nlohmann_json_default_obj.events;
    }
    std::for_each(events_ptr->begin(), events_ptr->end(), [&](const auto& event) {
        try {
            nlohmann_json_t.events.emplace_back(std::get<actor_created_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(std::get<skill_cast_begin_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(std::get<skill_cast_end_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(std::get<effect_application_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(std::get<damage_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(std::get<combat_stats_update_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
    });
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_REPORT_HPP
