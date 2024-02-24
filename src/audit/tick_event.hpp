#ifndef GW2COMBAT_AUDIT_TICK_EVENT_HPP
#define GW2COMBAT_AUDIT_TICK_EVENT_HPP

#include "common.hpp"

#include "events.hpp"

namespace gw2combat::audit {

struct tick_event_t {
    tick_t time_ms = 0;
    std::string actor;
    std::variant<actor_created_event_t,
                 skill_cast_begin_event_t,
                 skill_cast_end_event_t,
                 equipped_bundle_event_t,
                 dropped_bundle_event_t,
                 effect_application_event_t,
                 damage_event_t,
                 combat_stats_update_event_t,
                 effect_expired_event_t,
                 actor_downstate_event_t>
        event;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const tick_event_t& nlohmann_json_t) {
    nlohmann_json_j["time_ms"] = nlohmann_json_t.time_ms;
    nlohmann_json_j["actor"] = nlohmann_json_t.actor;
    std::visit([&](auto&& e) { nlohmann_json_j["event"] = e; }, nlohmann_json_t.event);
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, tick_event_t& nlohmann_json_t) {
    tick_event_t nlohmann_json_default_obj;
    nlohmann_json_t.time_ms = nlohmann_json_j.value("time_ms", nlohmann_json_default_obj.time_ms);
    nlohmann_json_t.actor = nlohmann_json_j.value("actor", nlohmann_json_default_obj.actor);
    if (nlohmann_json_j.contains("event")) {
        nlohmann_json_t.event =
            *((decltype(tick_event_t::event) const*)&nlohmann_json_j.at("event"));
    } else {
        nlohmann_json_t.event = nlohmann_json_default_obj.event;
    }
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_TICK_EVENT_HPP
