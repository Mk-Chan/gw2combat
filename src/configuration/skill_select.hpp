#ifndef GW2COMBAT_CONFIGURATION_SKILL_SELECT_HPP
#define GW2COMBAT_CONFIGURATION_SKILL_SELECT_HPP

#include "actor/skill.hpp"
#include "actor/weapon.hpp"
#include "actor/bundle.hpp"

namespace gw2combat::configuration {

struct skill_select_t {
    std::optional<actor::skill_t> skill_key = std::nullopt;
    std::optional<actor::skill_tag_t> tag = std::nullopt;
    std::optional<actor::weapon_type> weapon_type = std::nullopt;
    std::optional<actor::combo_field_t> combo_field = std::nullopt;
    std::optional<bool> can_critical_strike = std::nullopt;
    std::optional<bool> instant_cast_only_when_not_in_animation = std::nullopt;
    std::optional<actor::bundle_t> required_bundle = std::nullopt;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const skill_select_t& nlohmann_json_t) {
    if (nlohmann_json_t.skill_key) {
        nlohmann_json_j["skill_key"] = *nlohmann_json_t.skill_key;
    }
    if (nlohmann_json_t.tag) {
        nlohmann_json_j["tag"] = *nlohmann_json_t.tag;
    }
    if (nlohmann_json_t.weapon_type) {
        nlohmann_json_j["weapon_type"] = *nlohmann_json_t.weapon_type;
    }
    if (nlohmann_json_t.combo_field) {
        nlohmann_json_j["combo_field"] = *nlohmann_json_t.combo_field;
    }
    if (nlohmann_json_t.can_critical_strike) {
        nlohmann_json_j["can_critical_strike"] = *nlohmann_json_t.can_critical_strike;
    }
    if (nlohmann_json_t.instant_cast_only_when_not_in_animation) {
        nlohmann_json_j["instant_cast_only_when_not_in_animation"] =
            *nlohmann_json_t.instant_cast_only_when_not_in_animation;
    }
    if (nlohmann_json_t.required_bundle) {
        nlohmann_json_j["required_bundle"] = *nlohmann_json_t.required_bundle;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, skill_select_t& nlohmann_json_t) {
    skill_select_t nlohmann_json_default_obj;
    if (nlohmann_json_j.contains("skill_key")) {
        nlohmann_json_t.skill_key = nlohmann_json_j.value("skill_key", *nlohmann_json_default_obj.skill_key);
    }
    if (nlohmann_json_j.contains("tag")) {
        nlohmann_json_t.tag = nlohmann_json_j.value("tag", *nlohmann_json_default_obj.tag);
    }
    if (nlohmann_json_j.contains("weapon_type")) {
        nlohmann_json_t.weapon_type = nlohmann_json_j.value("weapon_type", *nlohmann_json_default_obj.weapon_type);
    }
    if (nlohmann_json_j.contains("combo_field")) {
        nlohmann_json_t.combo_field = nlohmann_json_j.value("combo_field", *nlohmann_json_default_obj.combo_field);
    }
    if (nlohmann_json_j.contains("can_critical_strike")) {
        nlohmann_json_t.can_critical_strike =
            nlohmann_json_j.value("can_critical_strike", *nlohmann_json_default_obj.can_critical_strike);
    }
    if (nlohmann_json_j.contains("instant_cast_only_when_not_in_animation")) {
        nlohmann_json_t.instant_cast_only_when_not_in_animation = nlohmann_json_j.value(
            "instant_cast_only_when_not_in_animation",
            *nlohmann_json_default_obj.instant_cast_only_when_not_in_animation);
    }
    if (nlohmann_json_j.contains("required_bundle")) {
        nlohmann_json_t.required_bundle =
            nlohmann_json_j.value("required_bundle", *nlohmann_json_default_obj.required_bundle);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_SKILL_SELECT_HPP
