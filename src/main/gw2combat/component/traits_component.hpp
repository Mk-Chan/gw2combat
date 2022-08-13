#ifndef GW2COMBAT_COMPONENT_TRAITS_HPP
#define GW2COMBAT_COMPONENT_TRAITS_HPP

#include "gw2combat/types.hpp"

namespace gw2combat {

enum class trait_line_type : std::uint32_t
{
    RADIANCE,
    VIRTUES,
    ZEAL,
    FIREBRAND,
    DRAGONHUNTER,
};

enum class trait_type : std::uint32_t
{
    NOTHING,

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

NLOHMANN_JSON_SERIALIZE_ENUM(trait_line_type,
                             {
                                 {trait_line_type::ZEAL, "Zeal"},
                                 {trait_line_type::RADIANCE, "Radiance"},
                                 {trait_line_type::VIRTUES, "Virtues"},
                                 {trait_line_type::FIREBRAND, "Firebrand"},
                                 {trait_line_type::DRAGONHUNTER, "Dragonhunter"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(trait_type,
                             {
                                 {trait_type::NOTHING, "Nothing"},

                                 {trait_type::RIGHT_HAND_STRENGTH, "Right-Hand Strength"},
                                 {trait_type::RADIANT_FIRE, "Radiant Fire"},
                                 {trait_type::RETRIBUTION, "Retribution"},
                                 {trait_type::AMPLIFIED_WRATH, "Amplified Wrath"},
                                 {trait_type::RADIANT_POWER, "Radiant Power"},
                                 {trait_type::RIGHTEOUS_INSTINCTS, "Righteous Instincts"},

                                 {trait_type::FIERY_WRATH, "Fiery Wrath"},
                                 {trait_type::ZEALOUS_BLADE, "Zealous Blade"},
                                 {trait_type::SYMBOLIC_POWER, "Symbolic Power"},
                                 {trait_type::SYMBOLIC_AVENGER, "Symbolic Avenger"},
                                 {trait_type::SYMBOLIC_EXPOSURE, "Symbolic Exposure"},

                                 {trait_type::POWER_OF_THE_VIRTUOUS, "Power of the Virtuous"},

                                 {trait_type::INSPIRED_VIRTUE, "Inspired Virtue"},
                                 {trait_type::UNSCATHED_CONTENDER, "Unscathed Contender"},
                                 {trait_type::MASTER_OF_CONSECRATIONS, "Master of Consecrations"},
                                 {trait_type::INSPIRING_VIRTUE, "Inspiring Virtue"},
                                 {trait_type::GLACIAL_HEART, "Glacial Heart"},
                                 {trait_type::PERMEATING_WRATH, "Permeating Wrath"},

                                 {trait_type::UNRELENTING_CRITICISM, "Unrelenting Criticism"},
                                 {trait_type::LEGENDARY_LORE, "Legendary Lore"},
                                 {trait_type::IMBUED_HASTE, "Imbued Haste"},
                                 {trait_type::STOIC_DEMEANOR, "Stoic Demeanor"},
                                 {trait_type::LOREMASTER, "Loremaster"},

                                 {trait_type::VIRTUOUS_ACTION, "Virtuous Action"},
                                 {trait_type::PIERCING_LIGHT, "Piercing Light"},
                                 {trait_type::ZEALOTS_AGGRESSION, "Zealot's Aggression"},
                                 {trait_type::PURE_OF_SIGHT, "Pure of Sight"},
                                 {trait_type::BIG_GAME_HUNTER, "Big Game Hunter"},
                             })

}  // namespace gw2combat

namespace gw2combat::component {

struct traits_component {
    std::vector<trait_type> traits;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_TRAITS_HPP
