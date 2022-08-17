#ifndef GW2COMBAT_TRAIT_HPP
#define GW2COMBAT_TRAIT_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::actor {

enum class trait_line_t : std::uint32_t
{
    INVALID,

    RADIANCE,
    VIRTUES,
    ZEAL,
    FIREBRAND,
    DRAGONHUNTER,
};

enum class trait_t : std::uint32_t
{
    INVALID,

    RIGHT_HAND_STRENGTH,
    RADIANT_FIRE,
    RETRIBUTION,
    AMPLIFIED_WRATH,
    RADIANT_POWER,
    RIGHTEOUS_INSTINCTS,

    FIERY_WRATH,
    ZEALOUS_BLADE,
    SYMBOLIC_POWER,
    SYMBOLIC_AVENGER,
    SYMBOLIC_EXPOSURE,

    INSPIRED_VIRTUE,
    UNSCATHED_CONTENDER,
    MASTER_OF_CONSECRATIONS,
    INSPIRING_VIRTUE,
    GLACIAL_HEART,
    PERMEATING_WRATH,

    POWER_OF_THE_VIRTUOUS,

    UNRELENTING_CRITICISM,
    LEGENDARY_LORE,
    IMBUED_HASTE,
    STOIC_DEMEANOR,
    LOREMASTER,

    VIRTUOUS_ACTION,
    PIERCING_LIGHT,
    ZEALOTS_AGGRESSION,
    PURE_OF_SIGHT,
    BIG_GAME_HUNTER,
};

struct trait_configuration_t {
    std::string name;
    std::vector<component::attribute_modification_t> attribute_modifications;
};

NLOHMANN_JSON_SERIALIZE_ENUM(trait_line_t,
                             {
                                 {trait_line_t::INVALID, "Invalid"},

                                 {trait_line_t::ZEAL, "Zeal"},
                                 {trait_line_t::RADIANCE, "Radiance"},
                                 {trait_line_t::VIRTUES, "Virtues"},
                                 {trait_line_t::FIREBRAND, "Firebrand"},
                                 {trait_line_t::DRAGONHUNTER, "Dragonhunter"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(trait_t,
                             {
                                 {trait_t::INVALID, "Invalid"},

                                 {trait_t::RIGHT_HAND_STRENGTH, "Right-Hand Strength"},
                                 {trait_t::RADIANT_FIRE, "Radiant Fire"},
                                 {trait_t::RETRIBUTION, "Retribution"},
                                 {trait_t::AMPLIFIED_WRATH, "Amplified Wrath"},
                                 {trait_t::RADIANT_POWER, "Radiant Power"},
                                 {trait_t::RIGHTEOUS_INSTINCTS, "Righteous Instincts"},

                                 {trait_t::FIERY_WRATH, "Fiery Wrath"},
                                 {trait_t::ZEALOUS_BLADE, "Zealous Blade"},
                                 {trait_t::SYMBOLIC_POWER, "Symbolic Power"},
                                 {trait_t::SYMBOLIC_AVENGER, "Symbolic Avenger"},
                                 {trait_t::SYMBOLIC_EXPOSURE, "Symbolic Exposure"},

                                 {trait_t::POWER_OF_THE_VIRTUOUS, "Power of the Virtuous"},

                                 {trait_t::INSPIRED_VIRTUE, "Inspired Virtue"},
                                 {trait_t::UNSCATHED_CONTENDER, "Unscathed Contender"},
                                 {trait_t::MASTER_OF_CONSECRATIONS, "Master of Consecrations"},
                                 {trait_t::INSPIRING_VIRTUE, "Inspiring Virtue"},
                                 {trait_t::GLACIAL_HEART, "Glacial Heart"},
                                 {trait_t::PERMEATING_WRATH, "Permeating Wrath"},

                                 {trait_t::UNRELENTING_CRITICISM, "Unrelenting Criticism"},
                                 {trait_t::LEGENDARY_LORE, "Legendary Lore"},
                                 {trait_t::IMBUED_HASTE, "Imbued Haste"},
                                 {trait_t::STOIC_DEMEANOR, "Stoic Demeanor"},
                                 {trait_t::LOREMASTER, "Loremaster"},

                                 {trait_t::VIRTUOUS_ACTION, "Virtuous Action"},
                                 {trait_t::PIERCING_LIGHT, "Piercing Light"},
                                 {trait_t::ZEALOTS_AGGRESSION, "Zealot's Aggression"},
                                 {trait_t::PURE_OF_SIGHT, "Pure of Sight"},
                                 {trait_t::BIG_GAME_HUNTER, "Big Game Hunter"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_TRAIT_HPP
