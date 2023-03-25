#ifndef GW2COMBAT_CONFIGURATION_CONDITION_HPP
#define GW2COMBAT_CONFIGURATION_CONDITION_HPP

#include "common.hpp"

#include "actor/effect.hpp"
#include "actor/skill.hpp"
#include "actor/unique_effect.hpp"
#include "actor/weapon.hpp"

#include "threshold.hpp"

namespace gw2combat::configuration {

struct condition_t {
    // NOTE: Remember to add conditions to to_json and from_json if adding a member to this
    // Independent of stage in combat loop
    std::optional<actor::weapon_type> weapon_type = std::nullopt;
    std::optional<actor::weapon_position> weapon_position = std::nullopt;
    std::optional<actor::weapon_set> weapon_set = std::nullopt;
    std::optional<actor::unique_effect_t> unique_effect_on_source = std::nullopt;
    std::optional<actor::effect_t> effect_on_source = std::nullopt;
    std::optional<actor::unique_effect_t> unique_effect_on_target = std::nullopt;
    std::optional<actor::unique_effect_t> unique_effect_on_target_by_source = std::nullopt;
    std::optional<actor::effect_t> effect_on_target = std::nullopt;
    std::optional<actor::skill_t> depends_on_skill_off_cooldown = std::nullopt;
    std::optional<threshold_t> threshold = std::nullopt;

    // composites
    std::vector<condition_t> not_conditions;
    std::vector<condition_t> or_conditions;
    std::vector<condition_t> and_conditions;

    // Dependent on combat stage
    // on-strike
    std::optional<bool> only_applies_on_strikes = std::nullopt;
    std::optional<bool> only_applies_on_critical_strikes = std::nullopt;
    std::optional<actor::skill_t> only_applies_on_strikes_by_skill = std::nullopt;
    std::optional<actor::skill_tag_t> only_applies_on_strikes_by_skill_with_tag = std::nullopt;

    // on-finished-casting
    std::optional<bool> only_applies_on_finished_casting = std::nullopt;
    std::optional<actor::skill_t> only_applies_on_finished_casting_skill = std::nullopt;
    std::optional<actor::skill_tag_t> only_applies_on_finished_casting_skill_with_tag =
        std::nullopt;
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const condition_t& nlohmann_json_t) {
    if (nlohmann_json_t.weapon_type) {
        nlohmann_json_j["weapon_type"] = *nlohmann_json_t.weapon_type;
    }
    if (nlohmann_json_t.weapon_position) {
        nlohmann_json_j["weapon_position"] = *nlohmann_json_t.weapon_position;
    }
    if (nlohmann_json_t.weapon_set) {
        nlohmann_json_j["weapon_set"] = *nlohmann_json_t.weapon_set;
    }
    if (nlohmann_json_t.unique_effect_on_source) {
        nlohmann_json_j["unique_effect_on_source"] = *nlohmann_json_t.unique_effect_on_source;
    }
    if (nlohmann_json_t.effect_on_source) {
        nlohmann_json_j["effect_on_source"] = *nlohmann_json_t.effect_on_source;
    }
    if (nlohmann_json_t.unique_effect_on_target) {
        nlohmann_json_j["unique_effect_on_target"] = *nlohmann_json_t.unique_effect_on_target;
    }
    if (nlohmann_json_t.unique_effect_on_target_by_source) {
        nlohmann_json_j["unique_effect_on_target_by_source"] =
            *nlohmann_json_t.unique_effect_on_target_by_source;
    }
    if (nlohmann_json_t.effect_on_target) {
        nlohmann_json_j["effect_on_target"] = *nlohmann_json_t.effect_on_target;
    }
    if (nlohmann_json_t.depends_on_skill_off_cooldown) {
        nlohmann_json_j["depends_on_skill_off_cooldown"] =
            *nlohmann_json_t.depends_on_skill_off_cooldown;
    }
    if (nlohmann_json_t.threshold) {
        nlohmann_json_j["threshold"] = *nlohmann_json_t.threshold;
    }
    if (!nlohmann_json_t.not_conditions.empty()) {
        nlohmann_json_j["not"] = nlohmann_json_t.not_conditions;
    }
    if (!nlohmann_json_t.or_conditions.empty()) {
        nlohmann_json_j["or"] = nlohmann_json_t.or_conditions;
    }
    if (!nlohmann_json_t.and_conditions.empty()) {
        nlohmann_json_j["and"] = nlohmann_json_t.and_conditions;
    }
    if (nlohmann_json_t.only_applies_on_strikes) {
        nlohmann_json_j["only_applies_on_strikes"] = *nlohmann_json_t.only_applies_on_strikes;
    }
    if (nlohmann_json_t.only_applies_on_critical_strikes) {
        nlohmann_json_j["only_applies_on_critical_strikes"] =
            *nlohmann_json_t.only_applies_on_critical_strikes;
    }
    if (nlohmann_json_t.only_applies_on_strikes_by_skill) {
        nlohmann_json_j["only_applies_on_strikes_by_skill"] =
            *nlohmann_json_t.only_applies_on_strikes_by_skill;
    }
    if (nlohmann_json_t.only_applies_on_strikes_by_skill_with_tag) {
        nlohmann_json_j["only_applies_on_strikes_by_skill_with_tag"] =
            *nlohmann_json_t.only_applies_on_strikes_by_skill_with_tag;
    }
    if (nlohmann_json_t.only_applies_on_finished_casting) {
        nlohmann_json_j["only_applies_on_finished_casting"] =
            *nlohmann_json_t.only_applies_on_finished_casting;
    }
    if (nlohmann_json_t.only_applies_on_finished_casting_skill) {
        nlohmann_json_j["only_applies_on_finished_casting_skill"] =
            *nlohmann_json_t.only_applies_on_finished_casting_skill;
    }
    if (nlohmann_json_t.only_applies_on_finished_casting_skill_with_tag) {
        nlohmann_json_j["only_applies_on_finished_casting_skill_with_tag"] =
            *nlohmann_json_t.only_applies_on_finished_casting_skill_with_tag;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, condition_t& nlohmann_json_t) {
    condition_t nlohmann_json_default_obj;
    if (nlohmann_json_j.contains("weapon_type")) {
        nlohmann_json_t.weapon_type =
            nlohmann_json_j.value("weapon_type", *nlohmann_json_default_obj.weapon_type);
    }
    if (nlohmann_json_j.contains("weapon_position")) {
        nlohmann_json_t.weapon_position =
            nlohmann_json_j.value("weapon_position", *nlohmann_json_default_obj.weapon_position);
    }
    if (nlohmann_json_j.contains("weapon_set")) {
        nlohmann_json_t.weapon_set =
            nlohmann_json_j.value("weapon_set", *nlohmann_json_default_obj.weapon_set);
    }
    if (nlohmann_json_j.contains("unique_effect_on_source")) {
        nlohmann_json_t.unique_effect_on_source = nlohmann_json_j.value(
            "unique_effect_on_source", *nlohmann_json_default_obj.unique_effect_on_source);
    }
    if (nlohmann_json_j.contains("effect_on_source")) {
        nlohmann_json_t.effect_on_source =
            nlohmann_json_j.value("effect_on_source", *nlohmann_json_default_obj.effect_on_source);
    }
    if (nlohmann_json_j.contains("unique_effect_on_target")) {
        nlohmann_json_t.unique_effect_on_target = nlohmann_json_j.value(
            "unique_effect_on_target", *nlohmann_json_default_obj.unique_effect_on_target);
    }
    if (nlohmann_json_j.contains("unique_effect_on_target_by_source")) {
        nlohmann_json_t.unique_effect_on_target_by_source =
            nlohmann_json_j.value("unique_effect_on_target_by_source",
                                  *nlohmann_json_default_obj.unique_effect_on_target_by_source);
    }
    if (nlohmann_json_j.contains("effect_on_target")) {
        nlohmann_json_t.effect_on_target =
            nlohmann_json_j.value("effect_on_target", *nlohmann_json_default_obj.effect_on_target);
    }
    if (nlohmann_json_j.contains("depends_on_skill_off_cooldown")) {
        nlohmann_json_t.depends_on_skill_off_cooldown =
            nlohmann_json_j.value("depends_on_skill_off_cooldown",
                                  *nlohmann_json_default_obj.depends_on_skill_off_cooldown);
    }
    if (nlohmann_json_j.contains("threshold")) {
        nlohmann_json_t.threshold =
            nlohmann_json_j.value("threshold", *nlohmann_json_default_obj.threshold);
    }
    if (nlohmann_json_j.contains("not")) {
        nlohmann_json_t.not_conditions =
            nlohmann_json_j.value("not", nlohmann_json_default_obj.not_conditions);
    }
    if (nlohmann_json_j.contains("or")) {
        nlohmann_json_t.or_conditions =
            nlohmann_json_j.value("or", nlohmann_json_default_obj.or_conditions);
    }
    if (nlohmann_json_j.contains("and")) {
        nlohmann_json_t.and_conditions =
            nlohmann_json_j.value("and", nlohmann_json_default_obj.and_conditions);
    }
    if (nlohmann_json_j.contains("only_applies_on_strikes")) {
        nlohmann_json_t.only_applies_on_strikes = nlohmann_json_j.value(
            "only_applies_on_strikes", *nlohmann_json_default_obj.only_applies_on_strikes);
    }
    if (nlohmann_json_j.contains("only_applies_on_critical_strikes")) {
        nlohmann_json_t.only_applies_on_critical_strikes =
            nlohmann_json_j.value("only_applies_on_critical_strikes",
                                  *nlohmann_json_default_obj.only_applies_on_critical_strikes);
    }
    if (nlohmann_json_j.contains("only_applies_on_strikes_by_skill")) {
        nlohmann_json_t.only_applies_on_strikes_by_skill =
            nlohmann_json_j.value("only_applies_on_strikes_by_skill",
                                  *nlohmann_json_default_obj.only_applies_on_strikes_by_skill);
    }
    if (nlohmann_json_j.contains("only_applies_on_strikes_by_skill_with_tag")) {
        nlohmann_json_t.only_applies_on_strikes_by_skill_with_tag = nlohmann_json_j.value(
            "only_applies_on_strikes_by_skill_with_tag",
            *nlohmann_json_default_obj.only_applies_on_strikes_by_skill_with_tag);
    }
    if (nlohmann_json_j.contains("only_applies_on_finished_casting")) {
        nlohmann_json_t.only_applies_on_finished_casting =
            nlohmann_json_j.value("only_applies_on_finished_casting",
                                  *nlohmann_json_default_obj.only_applies_on_finished_casting);
    }
    if (nlohmann_json_j.contains("only_applies_on_finished_casting_skill")) {
        nlohmann_json_t.only_applies_on_finished_casting_skill = nlohmann_json_j.value(
            "only_applies_on_finished_casting_skill",
            *nlohmann_json_default_obj.only_applies_on_finished_casting_skill);
    }
    if (nlohmann_json_j.contains("only_applies_on_finished_casting_skill_with_tag")) {
        nlohmann_json_t.only_applies_on_finished_casting_skill_with_tag = nlohmann_json_j.value(
            "only_applies_on_finished_casting_skill_with_tag",
            *nlohmann_json_default_obj.only_applies_on_finished_casting_skill_with_tag);
    }
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_CONDITION_HPP
