#ifndef GW2COMBAT_ACTOR_EFFECT_HPP
#define GW2COMBAT_ACTOR_EFFECT_HPP

#include "common.hpp"

namespace gw2combat::actor {

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
    BLINDED,
    CHILLED,
    CRIPPLED,
    FEAR,
    IMMOBILIZED,
    SLOW,
    TAUNT,
    WEAKNESS,
    VULNERABILITY,

    // Damaging conditions
    BURNING,
    BLEEDING,
    TORMENT,
    POISON,
    CONFUSION,

    // Unique
    BINDING_BLADE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(effect_t,
                             {
                                 {effect_t::INVALID, "invalid"},

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

                                 {effect_t::BLINDED, "BLINDED"},
                                 {effect_t::CHILLED, "CHILLED"},
                                 {effect_t::CRIPPLED, "CRIPPLED"},
                                 {effect_t::FEAR, "FEAR"},
                                 {effect_t::IMMOBILIZED, "IMMOBILIZED"},
                                 {effect_t::SLOW, "SLOW"},
                                 {effect_t::TAUNT, "TAUNT"},
                                 {effect_t::WEAKNESS, "WEAKNESS"},
                                 {effect_t::VULNERABILITY, "VULNERABILITY"},

                                 {effect_t::BURNING, "BURNING"},
                                 {effect_t::BLEEDING, "BLEEDING"},
                                 {effect_t::TORMENT, "TORMENT"},
                                 {effect_t::POISON, "POISON"},
                                 {effect_t::CONFUSION, "CONFUSION"},

                                 {effect_t::BINDING_BLADE, "BINDING_BLADE"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_EFFECT_HPP
