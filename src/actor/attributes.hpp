#ifndef GW2COMBAT_ACTOR_ATTRIBUTES_HPP
#define GW2COMBAT_ACTOR_ATTRIBUTES_HPP

#include "common.hpp"

namespace gw2combat::actor {

enum class attribute_t : std::uint8_t
{
    INVALID,

    POWER,
    PRECISION,
    TOUGHNESS,
    VITALITY,
    CONCENTRATION,
    CONDITION_DAMAGE,
    EXPERTISE,
    FEROCITY,
    HEALING_POWER,
    ARMOR,
    MAX_HEALTH,

    CRITICAL_CHANCE_MULTIPLIER,
    CRITICAL_DAMAGE_MULTIPLIER,

    BOON_DURATION_MULTIPLIER,
    AEGIS_DURATION_MULTIPLIER,
    ALACRITY_DURATION_MULTIPLIER,
    FURY_DURATION_MULTIPLIER,
    MIGHT_DURATION_MULTIPLIER,
    PROTECTION_DURATION_MULTIPLIER,
    QUICKNESS_DURATION_MULTIPLIER,
    REGENERATION_DURATION_MULTIPLIER,
    RESISTANCE_DURATION_MULTIPLIER,
    RESOLUTION_DURATION_MULTIPLIER,
    STABILITY_DURATION_MULTIPLIER,
    SWIFTNESS_DURATION_MULTIPLIER,
    VIGOR_DURATION_MULTIPLIER,

    CONDITION_DURATION_MULTIPLIER,
    BURNING_DURATION_MULTIPLIER,
    BLEEDING_DURATION_MULTIPLIER,
    CONFUSION_DURATION_MULTIPLIER,
    POISON_DURATION_MULTIPLIER,
    TORMENT_DURATION_MULTIPLIER,

    CONDITION_DAMAGE_MULTIPLIER,
    BURNING_DAMAGE_MULTIPLIER,
    BLEEDING_DAMAGE_MULTIPLIER,
    CONFUSION_DAMAGE_MULTIPLIER,
    POISON_DAMAGE_MULTIPLIER,
    TORMENT_DAMAGE_MULTIPLIER,

    OUTGOING_STRIKE_DAMAGE_MULTIPLIER,
    OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
    INCOMING_STRIKE_DAMAGE_MULTIPLIER,
    INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,

    OUTGOING_CONDITION_DAMAGE_MULTIPLIER,
    OUTGOING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
    INCOMING_CONDITION_DAMAGE_MULTIPLIER,
    INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    attribute_t,
    {
        {attribute_t::INVALID, "invalid"},

        {attribute_t::POWER, "power"},
        {attribute_t::PRECISION, "precision"},
        {attribute_t::TOUGHNESS, "toughness"},
        {attribute_t::VITALITY, "vitality"},
        {attribute_t::CONCENTRATION, "concentration"},
        {attribute_t::CONDITION_DAMAGE, "condition_damage"},
        {attribute_t::EXPERTISE, "expertise"},
        {attribute_t::FEROCITY, "ferocity"},
        {attribute_t::HEALING_POWER, "healing_power"},
        {attribute_t::ARMOR, "armor"},
        {attribute_t::MAX_HEALTH, "max_health"},

        {attribute_t::CRITICAL_CHANCE_MULTIPLIER, "critical_chance_multiplier"},
        {attribute_t::CRITICAL_DAMAGE_MULTIPLIER, "critical_damage_multiplier"},

        {attribute_t::BOON_DURATION_MULTIPLIER, "boon_duration_multiplier"},
        {attribute_t::AEGIS_DURATION_MULTIPLIER, "aegis_duration_multiplier"},
        {attribute_t::ALACRITY_DURATION_MULTIPLIER, "alacrity_duration_multiplier"},
        {attribute_t::FURY_DURATION_MULTIPLIER, "fury_duration_multiplier"},
        {attribute_t::MIGHT_DURATION_MULTIPLIER, "might_duration_multiplier"},
        {attribute_t::PROTECTION_DURATION_MULTIPLIER, "protection_duration_multiplier"},
        {attribute_t::QUICKNESS_DURATION_MULTIPLIER, "quickness_duration_multiplier"},
        {attribute_t::REGENERATION_DURATION_MULTIPLIER, "regeneration_duration_multiplier"},
        {attribute_t::RESISTANCE_DURATION_MULTIPLIER, "resistance_duration_multiplier"},
        {attribute_t::RESOLUTION_DURATION_MULTIPLIER, "resolution_duration_multiplier"},
        {attribute_t::STABILITY_DURATION_MULTIPLIER, "stability_duration_multiplier"},
        {attribute_t::SWIFTNESS_DURATION_MULTIPLIER, "swiftness_duration_multiplier"},
        {attribute_t::VIGOR_DURATION_MULTIPLIER, "vigor_duration_multiplier"},

        {attribute_t::CONDITION_DURATION_MULTIPLIER, "condition_duration_multiplier"},
        {attribute_t::BURNING_DURATION_MULTIPLIER, "burning_duration_multiplier"},
        {attribute_t::BLEEDING_DURATION_MULTIPLIER, "bleeding_duration_multiplier"},
        {attribute_t::CONFUSION_DURATION_MULTIPLIER, "confusion_duration_multiplier"},
        {attribute_t::POISON_DURATION_MULTIPLIER, "poison_duration_multiplier"},
        {attribute_t::TORMENT_DURATION_MULTIPLIER, "torment_duration_multiplier"},

        {attribute_t::CONDITION_DAMAGE_MULTIPLIER, "condition_damage_multiplier"},
        {attribute_t::BURNING_DAMAGE_MULTIPLIER, "burning_damage_multiplier"},
        {attribute_t::BLEEDING_DAMAGE_MULTIPLIER, "bleeding_damage_multiplier"},
        {attribute_t::CONFUSION_DAMAGE_MULTIPLIER, "confusion_damage_multiplier"},
        {attribute_t::POISON_DAMAGE_MULTIPLIER, "poison_damage_multiplier"},
        {attribute_t::TORMENT_DAMAGE_MULTIPLIER, "torment_damage_multiplier"},

        {attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER, "outgoing_strike_damage_multiplier"},
        {attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
         "outgoing_strike_damage_multiplier_add_group"},
        {attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER, "incoming_strike_damage_multiplier"},
        {attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
         "incoming_strike_damage_multiplier_add_group"},

        {attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER, "outgoing_condition_damage_multiplier"},
        {attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
         "outgoing_condition_damage_multiplier_add_group"},
        {attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER, "incoming_condition_damage_multiplier"},
        {attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
         "incoming_condition_damage_multiplier_add_group"},
    })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_ATTRIBUTES_HPP
