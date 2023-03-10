#ifndef GW2COMBAT_SYSTEM_AUDIT_HPP
#define GW2COMBAT_SYSTEM_AUDIT_HPP

#include "common.hpp"

#include "component/audit/audit_component.hpp"

namespace gw2combat::system {

struct audit_report_t {
    int remaining_health = -1;
    // NOTE: Don't forget to update to_json/from_json when adding more variants
    std::vector<std::variant<component::audit_damage_event_t,
                             component::audit_skill_cast_begin_event_t,
                             component::audit_skill_cast_end_event_t>>
        events;
};

extern void audit_damage(registry_t& registry);

extern void audit_report_to_disk(registry_t& registry);
extern audit_report_t get_audit_report(registry_t& registry);

static inline void to_json(nlohmann::json& nlohmann_json_j, const audit_report_t& nlohmann_json_t) {
    nlohmann_json_j["remaining_health"] = nlohmann_json_t.remaining_health;
    std::for_each(
        nlohmann_json_t.events.begin(), nlohmann_json_t.events.end(), [&](const auto& event) {
            if (std::holds_alternative<component::audit_damage_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(
                    std::get<component::audit_damage_event_t>(event));
            } else if (std::holds_alternative<component::audit_skill_cast_begin_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(
                    std::get<component::audit_skill_cast_begin_event_t>(event));
            } else if (std::holds_alternative<component::audit_skill_cast_end_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(
                    std::get<component::audit_skill_cast_end_event_t>(event));
            }
        });
}

static inline void from_json(const nlohmann::json& nlohmann_json_j,
                             audit_report_t& nlohmann_json_t) {
    audit_report_t nlohmann_json_default_obj;
    nlohmann_json_t.remaining_health =
        nlohmann_json_j.value("remaining_health", nlohmann_json_default_obj.remaining_health);

    std::vector<std::variant<component::audit_damage_event_t,
                             component::audit_skill_cast_begin_event_t,
                             component::audit_skill_cast_end_event_t>> const* events_ptr;
    if (nlohmann_json_j.contains("events")) {
        events_ptr =
            (std::vector<
                std::variant<component::audit_damage_event_t,
                             component::audit_skill_cast_begin_event_t,
                             component::audit_skill_cast_end_event_t>> const*)(&nlohmann_json_j
                                                                                    .at("events"));
    } else {
        events_ptr = &nlohmann_json_default_obj.events;
    }
    std::for_each(events_ptr->begin(), events_ptr->end(), [&](const auto& event) {
        try {
            nlohmann_json_t.events.emplace_back(std::get<component::audit_damage_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(
                std::get<component::audit_skill_cast_begin_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
        try {
            nlohmann_json_t.events.emplace_back(
                std::get<component::audit_skill_cast_end_event_t>(event));
            return;
        } catch (std::exception& ignored) {
        }
    });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_AUDIT_HPP
