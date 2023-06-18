#ifndef GW2COMBAT_CONFIGURATION_BUILD_HPP
#define GW2COMBAT_CONFIGURATION_BUILD_HPP

#include "common.hpp"

#include "actor/attributes.hpp"
#include "actor/base_class.hpp"
#include "actor/effect.hpp"
#include "actor/profession.hpp"
#include "actor/weapon.hpp"

#include "counter_configuration.hpp"
#include "skill.hpp"
#include "unique_effect.hpp"
#include "weapon.hpp"

namespace gw2combat::configuration {

struct build_t {
    actor::base_class_t base_class = actor::base_class_t::INVALID;
    actor::profession_t profession = actor::profession_t::INVALID;
    std::map<actor::attribute_t, double> attributes;
    std::vector<weapon_t> weapons{};
    actor::weapon_set initial_weapon_set = actor::weapon_set::SET_1;
    std::vector<skill_t> skills{};
    std::vector<actor::effect_t> permanent_effects{};
    std::vector<unique_effect_t> permanent_unique_effects{};
    std::vector<counter_configuration_t> counters{};

    build_t() {
        attributes[actor::attribute_t::POWER] = 1000.0;
        attributes[actor::attribute_t::PRECISION] = 1000.0;
        attributes[actor::attribute_t::TOUGHNESS] = 1000.0;
        attributes[actor::attribute_t::VITALITY] = 1000.0;
        attributes[actor::attribute_t::CONCENTRATION] = 0.0;
        attributes[actor::attribute_t::CONDITION_DAMAGE] = 0.0;
        attributes[actor::attribute_t::EXPERTISE] = 0.0;
        attributes[actor::attribute_t::FEROCITY] = 0.0;
        attributes[actor::attribute_t::HEALING_POWER] = 0.0;
        attributes[actor::attribute_t::ARMOR] = 1000.0;
        attributes[actor::attribute_t::MAX_HEALTH] = 1.0;

        attributes[actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER] = 0.0;
        attributes[actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER] = 1.5;

        attributes[actor::attribute_t::BOON_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::AEGIS_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::ALACRITY_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::FURY_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::MIGHT_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::PROTECTION_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::QUICKNESS_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::REGENERATION_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::RESISTANCE_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::RESOLUTION_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::STABILITY_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::SWIFTNESS_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::VIGOR_DURATION_MULTIPLIER] = 1.0;

        attributes[actor::attribute_t::CONDITION_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::BURNING_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::BLEEDING_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::CONFUSION_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::POISON_DURATION_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::TORMENT_DURATION_MULTIPLIER] = 1.0;

        attributes[actor::attribute_t::CONDITION_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::BURNING_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::BLEEDING_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::CONFUSION_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::POISON_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::TORMENT_DAMAGE_MULTIPLIER] = 1.0;

        attributes[actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP] = 0.0;
        attributes[actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP] = 0.0;

        attributes[actor::attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP] = 0.0;
        attributes[actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER] = 1.0;
        attributes[actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP] = 0.0;
    }
};

static inline void to_json(nlohmann::json& nlohmann_json_j, const build_t& nlohmann_json_t) {
    nlohmann_json_j["base_class"] = nlohmann_json_t.base_class;
    nlohmann_json_j["profession"] = nlohmann_json_t.profession;
    nlohmann_json_j["attributes"] = nlohmann_json_t.attributes;
    nlohmann_json_j["weapons"] = nlohmann_json_t.weapons;
    nlohmann_json_j["initial_weapon_set"] = nlohmann_json_t.initial_weapon_set;
    nlohmann_json_j["skills"] = nlohmann_json_t.skills;
    nlohmann_json_j["permanent_effects"] = nlohmann_json_t.permanent_effects;
    nlohmann_json_j["permanent_unique_effects"] = nlohmann_json_t.permanent_unique_effects;
    nlohmann_json_j["counters"] = nlohmann_json_t.counters;
}

static inline void from_json(const nlohmann::json& nlohmann_json_j, build_t& nlohmann_json_t) {
    build_t nlohmann_json_default_obj;
    nlohmann_json_t.base_class =
        nlohmann_json_j.value("base_class", nlohmann_json_default_obj.base_class);
    nlohmann_json_t.profession =
        nlohmann_json_j.value("profession", nlohmann_json_default_obj.profession);
    for (auto& entry : nlohmann_json_j.value("attributes", nlohmann_json_default_obj.attributes)) {
        nlohmann_json_t.attributes[entry.first] = entry.second;
    }
    nlohmann_json_t.weapons = nlohmann_json_j.value("weapons", nlohmann_json_default_obj.weapons);
    nlohmann_json_t.initial_weapon_set =
        nlohmann_json_j.value("initial_weapon_set", nlohmann_json_default_obj.initial_weapon_set);
    nlohmann_json_t.skills = nlohmann_json_j.value("skills", nlohmann_json_default_obj.skills);
    nlohmann_json_t.permanent_effects =
        nlohmann_json_j.value("permanent_effects", nlohmann_json_default_obj.permanent_effects);
    nlohmann_json_t.permanent_unique_effects = nlohmann_json_j.value(
        "permanent_unique_effects", nlohmann_json_default_obj.permanent_unique_effects);
    nlohmann_json_t.counters =
        nlohmann_json_j.value("counters", nlohmann_json_default_obj.counters);
}

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_BUILD_HPP
