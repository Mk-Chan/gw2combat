#ifndef GW2COMBAT_ATTRIBUTES_HPP
#define GW2COMBAT_ATTRIBUTES_HPP

#include "gw2combat/common.hpp"

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
    BOON_DURATION_PCT,
    CRITICAL_CHANCE_PCT,
    CRITICAL_DAMAGE_PCT,
    CONDITION_DURATION_PCT,
    BURNING_DURATION_PCT,
    BLEEDING_DURATION_PCT,
    CONFUSION_DURATION_PCT,
    POISON_DURATION_PCT,
    TORMENT_DURATION_PCT,
};

struct attributes_configuration_t {
    double power;
    double precision;
    double toughness;
    double vitality;
    double concentration;
    double condition_damage;
    double expertise;
    double ferocity;
    double healing_power;
    double armor;
    double max_health;
    double boon_duration_pct;
    double critical_chance_pct;
    double critical_damage_pct;
    double condition_duration_pct;
    double burning_duration_pct;
    double bleeding_duration_pct;
    double confusion_duration_pct;
    double poison_duration_pct;
    double torment_duration_pct;
};

NLOHMANN_JSON_SERIALIZE_ENUM(attribute_t,
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
                                 {attribute_t::BOON_DURATION_PCT, "boon_duration_pct"},
                                 {attribute_t::CRITICAL_CHANCE_PCT, "critical_chance_pct"},
                                 {attribute_t::CRITICAL_DAMAGE_PCT, "critical_damage_pct"},
                                 {attribute_t::CONDITION_DURATION_PCT, "condition_duration_pct"},
                                 {attribute_t::BURNING_DURATION_PCT, "burning_duration_pct"},
                                 {attribute_t::BLEEDING_DURATION_PCT, "bleeding_duration_pct"},
                                 {attribute_t::CONFUSION_DURATION_PCT, "confusion_duration_pct"},
                                 {attribute_t::POISON_DURATION_PCT, "poison_duration_pct"},
                                 {attribute_t::TORMENT_DURATION_PCT, "torment_duration_pct"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attributes_configuration_t,
                                                power,
                                                precision,
                                                toughness,
                                                vitality,
                                                concentration,
                                                condition_damage,
                                                expertise,
                                                ferocity,
                                                healing_power,
                                                armor,
                                                boon_duration_pct,
                                                critical_chance_pct,
                                                critical_damage_pct,
                                                condition_duration_pct,
                                                burning_duration_pct,
                                                bleeding_duration_pct,
                                                confusion_duration_pct,
                                                poison_duration_pct,
                                                torment_duration_pct,
                                                max_health)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ATTRIBUTES_HPP
