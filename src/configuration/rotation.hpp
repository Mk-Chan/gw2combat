#ifndef GW2COMBAT_CONFIGURATION_ROTATION_HPP
#define GW2COMBAT_CONFIGURATION_ROTATION_HPP

#include "common.hpp"

#include "actor/skill.hpp"
#include "skill_select.hpp"

namespace gw2combat::configuration {

struct skill_cast_t {
    actor::skill_t skill;
    std::optional<skill_select_t> skill_select = std::nullopt;
    tick_t cast_time_ms = 0;
};

struct rotation_t {
    std::vector<skill_cast_t> skill_casts;
    bool repeat = false;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const skill_cast_t& nlohmann_json_t) {
    nlohmann_json_j["skill"] = nlohmann_json_t.skill;
    if (nlohmann_json_t.skill_select) {
        nlohmann_json_j["skill_select"] = *nlohmann_json_t.skill_select;
    }
    nlohmann_json_j["cast_time_ms"] = nlohmann_json_t.cast_time_ms;
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, skill_cast_t& nlohmann_json_t) {
    skill_cast_t nlohmann_json_default_obj;
    nlohmann_json_t.skill = nlohmann_json_j.value("skill", nlohmann_json_default_obj.skill);
    if (nlohmann_json_j.contains("skill_select")) {
        nlohmann_json_t.skill_select = nlohmann_json_j.value("skill_select", *nlohmann_json_default_obj.skill_select);
    }
    nlohmann_json_t.cast_time_ms = nlohmann_json_j.value("cast_time_ms", nlohmann_json_default_obj.cast_time_ms);
}
static inline void to_json(nlohmann::json& nlohmann_json_j, const rotation_t& nlohmann_json_t) {
    nlohmann_json_j["skill_casts"] = nlohmann_json_t.skill_casts;
    nlohmann_json_j["repeat"] = nlohmann_json_t.repeat;
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, rotation_t& nlohmann_json_t) {
    rotation_t nlohmann_json_default_obj;
    nlohmann_json_t.skill_casts = nlohmann_json_j.value("skill_casts", nlohmann_json_default_obj.skill_casts);
    nlohmann_json_t.repeat = nlohmann_json_j.value("repeat", nlohmann_json_default_obj.repeat);
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ROTATION_HPP
