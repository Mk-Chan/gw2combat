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

    CRITICAL_CHANCE_PCT,
    CRITICAL_DAMAGE_PCT,

    BOON_DURATION_PCT,
    AEGIS_DURATION_PCT,
    ALACRITY_DURATION_PCT,
    FURY_DURATION_PCT,
    MIGHT_DURATION_PCT,
    PROTECTION_DURATION_PCT,
    QUICKNESS_DURATION_PCT,
    REGENERATION_DURATION_PCT,
    RESISTANCE_DURATION_PCT,
    RESOLUTION_DURATION_PCT,
    STABILITY_DURATION_PCT,
    SWIFTNESS_DURATION_PCT,
    VIGOR_DURATION_PCT,

    CONDITION_DURATION_PCT,
    BURNING_DURATION_PCT,
    BLEEDING_DURATION_PCT,
    CONFUSION_DURATION_PCT,
    POISON_DURATION_PCT,
    TORMENT_DURATION_PCT,

    CONDITION_DAMAGE_MULTIPLIER_PCT,
    BURNING_DAMAGE_MULTIPLIER_PCT,
    BLEEDING_DAMAGE_MULTIPLIER_PCT,
    CONFUSION_DAMAGE_MULTIPLIER_PCT,
    POISON_DAMAGE_MULTIPLIER_PCT,
    TORMENT_DAMAGE_MULTIPLIER_PCT,

    OUTGOING_STRIKE_DAMAGE_MULTIPLIER,
    OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
    INCOMING_STRIKE_DAMAGE_MULTIPLIER,
    INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
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

        {attribute_t::CRITICAL_CHANCE_PCT, "critical_chance_pct"},
        {attribute_t::CRITICAL_DAMAGE_PCT, "critical_damage_pct"},

        {attribute_t::BOON_DURATION_PCT, "boon_duration_pct"},
        {attribute_t::AEGIS_DURATION_PCT, "aegis_duration_pct"},
        {attribute_t::ALACRITY_DURATION_PCT, "alacrity_duration_pct"},
        {attribute_t::FURY_DURATION_PCT, "fury_duration_pct"},
        {attribute_t::MIGHT_DURATION_PCT, "might_duration_pct"},
        {attribute_t::PROTECTION_DURATION_PCT, "protection_duration_pct"},
        {attribute_t::QUICKNESS_DURATION_PCT, "quickness_duration_pct"},
        {attribute_t::REGENERATION_DURATION_PCT, "regeneration_duration_pct"},
        {attribute_t::RESISTANCE_DURATION_PCT, "resistance_duration_pct"},
        {attribute_t::RESOLUTION_DURATION_PCT, "resolution_duration_pct"},
        {attribute_t::STABILITY_DURATION_PCT, "stability_duration_pct"},
        {attribute_t::SWIFTNESS_DURATION_PCT, "swiftness_duration_pct"},
        {attribute_t::VIGOR_DURATION_PCT, "vigor_duration_pct"},

        {attribute_t::CONDITION_DURATION_PCT, "condition_duration_pct"},
        {attribute_t::BURNING_DURATION_PCT, "burning_duration_pct"},
        {attribute_t::BLEEDING_DURATION_PCT, "bleeding_duration_pct"},
        {attribute_t::CONFUSION_DURATION_PCT, "confusion_duration_pct"},
        {attribute_t::POISON_DURATION_PCT, "poison_duration_pct"},
        {attribute_t::TORMENT_DURATION_PCT, "torment_duration_pct"},

        {attribute_t::CONDITION_DAMAGE_MULTIPLIER_PCT, "condition_damage_multiplier_pct"},
        {attribute_t::BURNING_DAMAGE_MULTIPLIER_PCT, "burning_damage_multiplier_pct"},
        {attribute_t::BLEEDING_DAMAGE_MULTIPLIER_PCT, "bleeding_damage_multiplier_pct"},
        {attribute_t::CONFUSION_DAMAGE_MULTIPLIER_PCT, "confusion_damage_multiplier_pct"},
        {attribute_t::POISON_DAMAGE_MULTIPLIER_PCT, "poison_damage_multiplier_pct"},
        {attribute_t::TORMENT_DAMAGE_MULTIPLIER_PCT, "torment_damage_multiplier_pct"},

        {attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER, "outgoing_strike_damage_multiplier"},
        {attribute_t::OUTGOING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
         "outgoing_strike_damage_multiplier_add_group"},
        {attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER, "incoming_strike_damage_multiplier"},
        {attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
         "incoming_strike_damage_multiplier_add_group"},
    })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_ATTRIBUTES_HPP
