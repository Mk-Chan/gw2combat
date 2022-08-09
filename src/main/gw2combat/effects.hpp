#ifndef GW2COMBAT_EFFECT_HPP
#define GW2COMBAT_EFFECT_HPP

#include <numeric>

#include <entt/entt.hpp>

#include "types.hpp"

#include "gw2combat/component/character/effective_attributes.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_condition_damage_multiplier.hpp"

namespace gw2combat::effects {

constexpr inline unsigned int effect_pulse_rate = 1'000;

enum class effect_type : std::uint32_t
{
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
};

enum class effect_stacking_type : std::uint8_t
{
    STACKING_INTENSITY,
    STACKING_DURATION,
};

struct effect_stack {
    [[nodiscard]] effect_stack(entity_t source, tick_t duration)
        : source(source), duration(duration) {
    }
    entity_t source;
    tick_t duration;
    tick_t progress = 0;
};

struct effect_application {
    [[nodiscard]] effect_application(effect_type effect_type, entity_t source, tick_t duration)
        : effect_type(effect_type), source(source), duration(duration), num_stacks(1) {
    }
    [[nodiscard]] effect_application(effect_type effect_type,
                                     entity_t source,
                                     tick_t duration,
                                     size_t num_stacks)
        : effect_type(effect_type), source(source), duration(duration), num_stacks(num_stacks) {
    }
    effect_type effect_type;
    entity_t source;
    tick_t duration;
    size_t num_stacks;
};

NLOHMANN_JSON_SERIALIZE_ENUM(effect_type,
                             {
                                 {effect_type::AEGIS, "AEGIS"},
                                 {effect_type::ALACRITY, "ALACRITY"},
                                 {effect_type::FURY, "FURY"},
                                 {effect_type::MIGHT, "MIGHT"},
                                 {effect_type::QUICKNESS, "QUICKNESS"},
                                 {effect_type::RESOLUTION, "RESOLUTION"},
                                 {effect_type::RESISTANCE, "RESISTANCE"},
                                 {effect_type::PROTECTION, "PROTECTION"},
                                 {effect_type::REGENERATION, "REGENERATION"},
                                 {effect_type::VIGOR, "VIGOR"},
                                 {effect_type::SWIFTNESS, "SWIFTNESS"},
                                 {effect_type::STABILITY, "STABILITY"},

                                 {effect_type::VULNERABILITY, "VULNERABILITY"},

                                 {effect_type::BURNING, "BURNING"},
                                 {effect_type::BLEEDING, "BLEEDING"},
                                 {effect_type::TORMENT, "TORMENT"},
                                 {effect_type::POISON, "POISON"},
                                 {effect_type::CONFUSION, "CONFUSION"},

                                 {effect_type::BINDING_BLADE, "BINDING_BLADE"},
                                 {effect_type::VIRTUE_OF_JUSTICE, "VIRTUE_OF_JUSTICE"},
                                 {effect_type::SYMBOLIC_AVENGER, "SYMBOLIC_AVENGER"},
                                 {effect_type::INSPIRING_VIRTUE, "INSPIRING_VIRTUE"},
                                 {effect_type::ASHES_OF_THE_JUST, "ASHES_OF_THE_JUST"},
                             })

}  // namespace gw2combat::effects

#endif  // GW2COMBAT_EFFECT_HPP
