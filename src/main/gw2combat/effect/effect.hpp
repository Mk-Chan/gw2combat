#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <numeric>

#include "gw2combat/common.hpp"

namespace gw2combat::effect {

enum class effect_t : std::uint32_t
{
    INVALID,

    // Boons
    AEGIS,
    ALACRITY,
    FURY,
    MIGHT,
    QUICKNESS,
    RESOLUTION,
    RESISTANCE,
    PROTECTION,
    REGENERATION,
    VIGOR,
    SWIFTNESS,
    STABILITY,

    // Non-damaging conditions
    VULNERABILITY,
    CRIPPLED,

    // Damaging conditions
    BURNING,
    BLEEDING,
    TORMENT,
    POISON,
    CONFUSION,

    // Unique
    BINDING_BLADE,
    VIRTUE_OF_JUSTICE,
    SYMBOLIC_AVENGER,
    INSPIRING_VIRTUE,
    ASHES_OF_THE_JUST,

    ARBITRARY_EFFECT,
};

NLOHMANN_JSON_SERIALIZE_ENUM(effect_t,
                             {
                                 {effect_t::INVALID, "Invalid"},

                                 {effect_t::AEGIS, "AEGIS"},
                                 {effect_t::ALACRITY, "ALACRITY"},
                                 {effect_t::FURY, "FURY"},
                                 {effect_t::MIGHT, "MIGHT"},
                                 {effect_t::QUICKNESS, "QUICKNESS"},
                                 {effect_t::RESOLUTION, "RESOLUTION"},
                                 {effect_t::RESISTANCE, "RESISTANCE"},
                                 {effect_t::PROTECTION, "PROTECTION"},
                                 {effect_t::REGENERATION, "REGENERATION"},
                                 {effect_t::VIGOR, "VIGOR"},
                                 {effect_t::SWIFTNESS, "SWIFTNESS"},
                                 {effect_t::STABILITY, "STABILITY"},

                                 {effect_t::VULNERABILITY, "VULNERABILITY"},
                                 {effect_t::CRIPPLED, "CRIPPLED"},

                                 {effect_t::BURNING, "BURNING"},
                                 {effect_t::BLEEDING, "BLEEDING"},
                                 {effect_t::TORMENT, "TORMENT"},
                                 {effect_t::POISON, "POISON"},
                                 {effect_t::CONFUSION, "CONFUSION"},

                                 {effect_t::BINDING_BLADE, "BINDING_BLADE"},
                                 {effect_t::VIRTUE_OF_JUSTICE, "VIRTUE_OF_JUSTICE"},
                                 {effect_t::SYMBOLIC_AVENGER, "SYMBOLIC_AVENGER"},
                                 {effect_t::INSPIRING_VIRTUE, "INSPIRING_VIRTUE"},
                                 {effect_t::ASHES_OF_THE_JUST, "ASHES_OF_THE_JUST"},

                                 {effect_t::ARBITRARY_EFFECT, "ARBITRARY_EFFECT"},
                             })

}  // namespace gw2combat::effect

#endif  // GW2COMBAT_EFFECT_HPP
