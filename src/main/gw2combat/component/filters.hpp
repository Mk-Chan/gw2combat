#ifndef GW2COMBAT_COMPONENT_FILTERS_HPP
#define GW2COMBAT_COMPONENT_FILTERS_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/actor/rune.hpp"
#include "gw2combat/actor/skill.hpp"
#include "gw2combat/actor/trait.hpp"
#include "gw2combat/actor/unique_effect.hpp"
#include "gw2combat/actor/weapon.hpp"
#include "gw2combat/effect/effect.hpp"
#include "gw2combat/strike/strike.hpp"

namespace gw2combat::component {

struct strike_counter_configuration_t {
    int required_count = 1;

    [[nodiscard]] inline bool operator==(const strike_counter_configuration_t& rhs) const {
        return required_count == rhs.required_count;
    }
};

struct filters_t {
    std::optional<actor::rune_t> rune = std::nullopt;
    std::optional<actor::weapon_type> weapon_type = std::nullopt;
    std::optional<actor::weapon_position> weapon_position = std::nullopt;
    std::optional<actor::weapon_sigil> weapon_sigil = std::nullopt;
    std::optional<actor::trait_t> source_trait = std::nullopt;
    std::optional<effect::effect_t> source_actor_effect = std::nullopt;
    std::optional<actor::unique_effect_t> source_actor_unique_effect = std::nullopt;
    std::optional<actor::skill_t> source_skill = std::nullopt;
    std::optional<actor::skill_tag_t> source_skill_tag = std::nullopt;
    std::optional<effect::effect_t> target_actor_effect = std::nullopt;
    std::optional<actor::unique_effect_t> target_actor_unique_effect = std::nullopt;
    std::optional<actor::skill_t> depends_on_skill_off_cooldown = std::nullopt;
    std::optional<strike_counter_configuration_t> strike_counter_configuration = std::nullopt;

    [[nodiscard]] inline bool operator==(const filters_t& rhs) const {
        return rune == rhs.rune && weapon_type == rhs.weapon_type &&
               weapon_position == rhs.weapon_position && weapon_sigil == rhs.weapon_sigil &&
               source_trait == rhs.source_trait && source_actor_effect == rhs.source_actor_effect &&
               source_actor_unique_effect == rhs.source_actor_unique_effect &&
               source_skill == rhs.source_skill && source_skill_tag == rhs.source_skill_tag &&
               target_actor_effect == rhs.target_actor_effect &&
               target_actor_unique_effect == rhs.target_actor_unique_effect &&
               depends_on_skill_off_cooldown == rhs.depends_on_skill_off_cooldown &&
               strike_counter_configuration == rhs.strike_counter_configuration;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(strike_counter_configuration_t, required_count)

static inline void to_json(nlohmann::json& nlohmann_json_j, const filters_t& nlohmann_json_t) {
    if (nlohmann_json_t.rune) {
        nlohmann_json_j["rune"] = *nlohmann_json_t.rune;
    }
    if (nlohmann_json_t.weapon_type) {
        nlohmann_json_j["weapon_type"] = *nlohmann_json_t.weapon_type;
    }
    if (nlohmann_json_t.weapon_position) {
        nlohmann_json_j["weapon_position"] = *nlohmann_json_t.weapon_position;
    }
    if (nlohmann_json_t.weapon_sigil) {
        nlohmann_json_j["weapon_sigil"] = *nlohmann_json_t.weapon_sigil;
    }
    if (nlohmann_json_t.source_trait) {
        nlohmann_json_j["source_trait"] = *nlohmann_json_t.source_trait;
    }
    if (nlohmann_json_t.source_actor_effect) {
        nlohmann_json_j["source_actor_effect"] = *nlohmann_json_t.source_actor_effect;
    }
    if (nlohmann_json_t.source_actor_unique_effect) {
        nlohmann_json_j["source_actor_unique_effect"] = *nlohmann_json_t.source_actor_unique_effect;
    }
    if (nlohmann_json_t.source_skill) {
        nlohmann_json_j["source_skill"] = *nlohmann_json_t.source_skill;
    }
    if (nlohmann_json_t.source_skill_tag) {
        nlohmann_json_j["source_skill_tag"] = *nlohmann_json_t.source_skill_tag;
    }
    if (nlohmann_json_t.target_actor_effect) {
        nlohmann_json_j["target_actor_effect"] = *nlohmann_json_t.target_actor_effect;
    }
    if (nlohmann_json_t.target_actor_unique_effect) {
        nlohmann_json_j["target_actor_unique_effect"] = *nlohmann_json_t.target_actor_unique_effect;
    }
    if (nlohmann_json_t.depends_on_skill_off_cooldown) {
        nlohmann_json_j["depends_on_skill_off_cooldown"] =
            *nlohmann_json_t.depends_on_skill_off_cooldown;
    }
    if (nlohmann_json_t.strike_counter_configuration) {
        nlohmann_json_j["strike_counter_configuration"] =
            *nlohmann_json_t.strike_counter_configuration;
    }
}
static inline void from_json(const nlohmann::json& nlohmann_json_j, filters_t& nlohmann_json_t) {
    filters_t nlohmann_json_default_obj;
    if (nlohmann_json_j.contains("rune")) {
        nlohmann_json_t.rune = nlohmann_json_j.value("rune", *nlohmann_json_default_obj.rune);
    }
    if (nlohmann_json_j.contains("weapon_type")) {
        nlohmann_json_t.weapon_type =
            nlohmann_json_j.value("weapon_type", *nlohmann_json_default_obj.weapon_type);
    }
    if (nlohmann_json_j.contains("weapon_position")) {
        nlohmann_json_t.weapon_position =
            nlohmann_json_j.value("weapon_position", *nlohmann_json_default_obj.weapon_position);
    }
    if (nlohmann_json_j.contains("weapon_sigil")) {
        nlohmann_json_t.weapon_sigil =
            nlohmann_json_j.value("weapon_sigil", *nlohmann_json_default_obj.weapon_sigil);
    }
    if (nlohmann_json_j.contains("source_trait")) {
        nlohmann_json_t.source_trait =
            nlohmann_json_j.value("source_trait", *nlohmann_json_default_obj.source_trait);
    }
    if (nlohmann_json_j.contains("source_actor_effect")) {
        nlohmann_json_t.source_actor_effect = nlohmann_json_j.value(
            "source_actor_effect", *nlohmann_json_default_obj.source_actor_effect);
    }
    if (nlohmann_json_j.contains("source_actor_unique_effect")) {
        nlohmann_json_t.source_actor_unique_effect = nlohmann_json_j.value(
            "source_actor_unique_effect", *nlohmann_json_default_obj.source_actor_unique_effect);
    }
    if (nlohmann_json_j.contains("source_skill")) {
        nlohmann_json_t.source_skill =
            nlohmann_json_j.value("source_skill", *nlohmann_json_default_obj.source_skill);
    }
    if (nlohmann_json_j.contains("source_skill_tag")) {
        nlohmann_json_t.source_skill_tag =
            nlohmann_json_j.value("source_skill_tag", *nlohmann_json_default_obj.source_skill_tag);
    }
    if (nlohmann_json_j.contains("target_actor_effect")) {
        nlohmann_json_t.target_actor_effect = nlohmann_json_j.value(
            "target_actor_effect", *nlohmann_json_default_obj.target_actor_effect);
    }
    if (nlohmann_json_j.contains("target_actor_unique_effect")) {
        nlohmann_json_t.target_actor_unique_effect = nlohmann_json_j.value(
            "target_actor_unique_effect", *nlohmann_json_default_obj.target_actor_unique_effect);
    }
    if (nlohmann_json_j.contains("depends_on_skill_off_cooldown")) {
        nlohmann_json_t.depends_on_skill_off_cooldown =
            nlohmann_json_j.value("depends_on_skill_off_cooldown",
                                  *nlohmann_json_default_obj.depends_on_skill_off_cooldown);
    }
    if (nlohmann_json_j.contains("strike_counter_configuration")) {
        nlohmann_json_t.strike_counter_configuration =
            nlohmann_json_j.value("strike_counter_configuration",
                                  *nlohmann_json_default_obj.strike_counter_configuration);
    }
}

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_FILTERS_HPP
