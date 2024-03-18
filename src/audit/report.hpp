#ifndef GW2COMBAT_AUDIT_REPORT_HPP
#define GW2COMBAT_AUDIT_REPORT_HPP

#include "common.hpp"

#include "tick_event.hpp"

#include "actor/bundle.hpp"
#include "actor/counter.hpp"
#include "actor/effect.hpp"
#include "actor/skill.hpp"
#include "actor/unique_effect.hpp"
#include "actor/weapon.hpp"

namespace gw2combat::audit {

struct counter_value_t {
    actor::counter_t counter;
    int value = 0;
};

struct uncastable_skill_t {
    std::string reason;
    int remaining_cooldown = 0;
    int remaining_ammo = 0;
};

struct actor_effect_t {
    actor::effect_t effect = actor::effect_t::INVALID;
    std::string source_actor;
    int remaining_duration = 0;
};

struct actor_effect_summary_t {
    int stacks = 0;
    int remaining_duration = 0;
};

struct actor_unique_effect_t {
    actor::unique_effect_t unique_effect;
    std::string source_actor;
    int remaining_duration = 0;
};

struct actor_unique_effect_summary_t {
    int stacks = 0;
    int remaining_duration = 0;
};

struct report_t {
    int offset = 0;
    std::vector<tick_event_t> tick_events;
    std::optional<std::string> error;
    std::vector<counter_value_t> counter_values;
    std::map<std::string, std::vector<std::string>> castable_skills_by_actor;
    std::map<std::string, std::map<actor::skill_t, uncastable_skill_t>> uncastable_skills_by_actor;
    std::map<std::string, actor::weapon_set> current_weapon_set_by_actor;
    std::map<std::string, actor::bundle_t> current_bundle_by_actor;
    std::map<std::string, std::map<std::string, actor_effect_summary_t>> effects_by_actor;
    std::map<std::string, std::map<actor::unique_effect_t, actor_unique_effect_summary_t>>
        unique_effects_by_actor;
    std::map<std::string, std::map<std::string, double>> attributes_by_actor;
    std::map<std::string, int> afk_ticks_by_actor;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(counter_value_t, counter, value)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(uncastable_skill_t,
                                                reason,
                                                remaining_cooldown,
                                                remaining_ammo)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_effect_t,
                                                effect,
                                                source_actor,
                                                remaining_duration)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_effect_summary_t, stacks, remaining_duration)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_unique_effect_t,
                                                unique_effect,
                                                source_actor,
                                                remaining_duration)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_unique_effect_summary_t,
                                                stacks,
                                                remaining_duration)

static inline void to_json(nlohmann::json& nlohmann_json_j, const report_t& nlohmann_json_t) {
    nlohmann_json_j["offset"] = nlohmann_json_t.offset;
    std::copy(nlohmann_json_t.tick_events.cbegin(),
              nlohmann_json_t.tick_events.cend(),
              std::back_inserter(nlohmann_json_j["tick_events"]));
    if (nlohmann_json_t.error) {
        nlohmann_json_j["error"] = *nlohmann_json_t.error;
    }
    nlohmann_json_j["counter_values"] = nlohmann_json_t.counter_values;
    nlohmann_json_j["castable_skills_by_actor"] = nlohmann_json_t.castable_skills_by_actor;
    nlohmann_json_j["uncastable_skills_by_actor"] = nlohmann_json_t.uncastable_skills_by_actor;
    nlohmann_json_j["current_weapon_set_by_actor"] = nlohmann_json_t.current_weapon_set_by_actor;
    nlohmann_json_j["current_bundle_by_actor"] = nlohmann_json_t.current_bundle_by_actor;
    nlohmann_json_j["effects_by_actor"] = nlohmann_json_t.effects_by_actor;
    nlohmann_json_j["unique_effects_by_actor"] = nlohmann_json_t.unique_effects_by_actor;
    nlohmann_json_j["attributes_by_actor"] = nlohmann_json_t.attributes_by_actor;
    nlohmann_json_j["afk_ticks_by_actor"] = nlohmann_json_t.afk_ticks_by_actor;
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, report_t& nlohmann_json_t) {
    report_t nlohmann_json_default_obj;
    nlohmann_json_t.offset = nlohmann_json_j.value("offset", nlohmann_json_default_obj.offset);
    nlohmann_json_t.tick_events =
        nlohmann_json_j.value("tick_events", nlohmann_json_default_obj.tick_events);
    if (nlohmann_json_j.contains("error")) {
        nlohmann_json_t.error = nlohmann_json_j.value("error", *nlohmann_json_default_obj.error);
    }
    nlohmann_json_t.counter_values =
        nlohmann_json_j.value("counter_values", nlohmann_json_default_obj.counter_values);
    nlohmann_json_t.castable_skills_by_actor = nlohmann_json_j.value(
        "castable_skills_by_actor", nlohmann_json_default_obj.castable_skills_by_actor);
    nlohmann_json_t.uncastable_skills_by_actor = nlohmann_json_j.value(
        "uncastable_skills_by_actor", nlohmann_json_default_obj.uncastable_skills_by_actor);
    nlohmann_json_t.current_weapon_set_by_actor = nlohmann_json_j.value(
        "current_weapon_set_by_actor", nlohmann_json_default_obj.current_weapon_set_by_actor);
    nlohmann_json_t.current_bundle_by_actor = nlohmann_json_j.value(
        "current_bundle_by_actor", nlohmann_json_default_obj.current_bundle_by_actor);
    nlohmann_json_t.effects_by_actor =
        nlohmann_json_j.value("effects_by_actor", nlohmann_json_default_obj.effects_by_actor);
    nlohmann_json_t.unique_effects_by_actor = nlohmann_json_j.value(
        "unique_effects_by_actor", nlohmann_json_default_obj.unique_effects_by_actor);
    nlohmann_json_t.attributes_by_actor =
        nlohmann_json_j.value("attributes_by_actor", nlohmann_json_default_obj.attributes_by_actor);
    nlohmann_json_t.afk_ticks_by_actor =
        nlohmann_json_j.value("afk_ticks_by_actor", nlohmann_json_default_obj.afk_ticks_by_actor);
}

}  // namespace gw2combat::audit

#endif  // GW2COMBAT_AUDIT_REPORT_HPP
