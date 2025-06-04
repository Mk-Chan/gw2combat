#ifndef GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP
#define GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP

#include "actor/skill.hpp"
#include "skill_select.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct skill_trigger_t {
    condition_t condition;

    actor::skill_t skill_key;
    std::optional<skill_select_t> skill_select = std::nullopt;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const skill_trigger_t& nlohmann_json_t) {
    nlohmann_json_j["condition"] = nlohmann_json_t.condition;
    nlohmann_json_j["skill_key"] = nlohmann_json_t.skill_key;
    if (nlohmann_json_t.skill_select) {
        nlohmann_json_j["skill_select"] = *nlohmann_json_t.skill_select;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, skill_trigger_t& nlohmann_json_t) {
    skill_trigger_t nlohmann_json_default_obj;
    nlohmann_json_t.condition = nlohmann_json_j.value("condition", nlohmann_json_default_obj.condition);
    nlohmann_json_t.skill_key = nlohmann_json_j.value("skill_key", nlohmann_json_default_obj.skill_key);
    if (nlohmann_json_j.contains("skill_select")) {
        nlohmann_json_t.skill_select = nlohmann_json_j.value("skill_select", *nlohmann_json_default_obj.skill_select);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP
