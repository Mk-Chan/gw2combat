#ifndef GW2COMBAT_SYSTEM_AUDIT_HPP
#define GW2COMBAT_SYSTEM_AUDIT_HPP

#include "common.hpp"

#include <variant>

namespace gw2combat::system {

struct audit_report_t {
    enum class event_type_t
    {
        DAMAGE_EVENT,
    };

    struct damage_event_t {
        enum class damage_type_t
        {
            INVALID,

            STRIKE,
            BINDING_BLADE,
            BLEEDING,
            BURNING,
            CONFUSION,
            CONFUSION_ON_SKILL_ACTIVATION,
            POISON,
            TORMENT_STATIONARY,
            TORMENT_MOVING,
        };

        event_type_t event_type = event_type_t::DAMAGE_EVENT;
        tick_t time_ms;
        std::string actor;
        std::string source_skill;
        damage_type_t damage_type;
        int damage;
    };

    int remaining_health;
    // NOTE: Don't forget to update to_json/from_json when adding more variants
    std::vector<std::variant<damage_event_t>> events;
};

extern void audit_damage(registry_t& registry);

extern void audit_report_to_disk(registry_t& registry);
extern audit_report_t get_audit_report(registry_t& registry);

NLOHMANN_JSON_SERIALIZE_ENUM(audit_report_t::event_type_t,
                             {
                                 {audit_report_t::event_type_t::DAMAGE_EVENT, "damage_event"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(
    audit_report_t::damage_event_t::damage_type_t,
    {
        {audit_report_t::damage_event_t::damage_type_t::INVALID, "invalid"},
        {audit_report_t::damage_event_t::damage_type_t::STRIKE, "strike"},
        {audit_report_t::damage_event_t::damage_type_t::BINDING_BLADE, "binding_blade"},
        {audit_report_t::damage_event_t::damage_type_t::BLEEDING, "bleeding"},
        {audit_report_t::damage_event_t::damage_type_t::BURNING, "burning"},
        {audit_report_t::damage_event_t::damage_type_t::CONFUSION, "confusion"},
        {audit_report_t::damage_event_t::damage_type_t::CONFUSION_ON_SKILL_ACTIVATION,
         "confusion_on_skill_activation"},
        {audit_report_t::damage_event_t::damage_type_t::POISON, "poison"},
        {audit_report_t::damage_event_t::damage_type_t::TORMENT_STATIONARY, "torment_stationary"},
        {audit_report_t::damage_event_t::damage_type_t::TORMENT_MOVING, "torment_moving"},
    })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(audit_report_t::damage_event_t,
                                                event_type,
                                                actor,
                                                source_skill,
                                                damage_type,
                                                time_ms,
                                                damage)
static inline void to_json(nlohmann::json& nlohmann_json_j, const audit_report_t& nlohmann_json_t) {
    nlohmann_json_j["remaining_health"] = nlohmann_json_t.remaining_health;
    std::for_each(
        nlohmann_json_t.events.begin(), nlohmann_json_t.events.end(), [&](const auto& event) {
            if (std::holds_alternative<audit_report_t::damage_event_t>(event)) {
                nlohmann_json_j["events"].emplace_back(
                    std::get<audit_report_t::damage_event_t>(event));
            }
        });
}

static inline void from_json(const nlohmann::json& nlohmann_json_j,
                             audit_report_t& nlohmann_json_t) {
    audit_report_t nlohmann_json_default_obj;
    nlohmann_json_t.remaining_health =
        nlohmann_json_j.value("remaining_health", nlohmann_json_default_obj.remaining_health);

    std::vector<std::variant<audit_report_t::damage_event_t>> const* events_ptr;
    if (nlohmann_json_j.contains("events")) {
        events_ptr =
            (std::vector<std::variant<audit_report_t::damage_event_t>> const*)(&nlohmann_json_j.at(
                "events"));
    } else {
        events_ptr = &nlohmann_json_default_obj.events;
    }
    std::for_each(events_ptr->begin(), events_ptr->end(), [&](const auto& event) {
        try {
            nlohmann_json_t.events.emplace_back(std::get<audit_report_t::damage_event_t>(event));
        } catch (std::exception& ignored) {
        }
    });
}

}  // namespace gw2combat::system

#endif  // GW2COMBAT_SYSTEM_AUDIT_HPP
