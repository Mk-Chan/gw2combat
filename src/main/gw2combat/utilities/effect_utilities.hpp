#ifndef GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/effect/effect.hpp"
#include "gw2combat/effect/stacking.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"

namespace gw2combat::utils {

struct effect_duration_pct_addends_t {
    double boon_duration_pct_addend = 0.0;
    double condition_duration_pct_addend = 0.0;
    double burning_duration_pct_addend = 0.0;
    double bleeding_duration_pct_addend = 0.0;
    double confusion_duration_pct_addend = 0.0;
    double poison_duration_pct_addend = 0.0;
    double torment_duration_pct_addend = 0.0;
};

constexpr static inline std::array BOONS{
    effect::effect_t::AEGIS,
    effect::effect_t::ALACRITY,
    effect::effect_t::FURY,
    effect::effect_t::MIGHT,
    effect::effect_t::QUICKNESS,
    effect::effect_t::RESOLUTION,
    effect::effect_t::RESISTANCE,
    effect::effect_t::PROTECTION,
    effect::effect_t::REGENERATION,
    effect::effect_t::VIGOR,
    effect::effect_t::SWIFTNESS,
    effect::effect_t::STABILITY,
};

[[nodiscard]] static inline effect::stacking_t get_effect_stacking_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::CRIPPLED:
            return effect::stacking_t::STACKING_DURATION;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return effect::stacking_t::STACKING_INTENSITY;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_stored_stacks_of_effect_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::CRIPPLED:
            return 1;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1500;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_considered_stacks_of_effect_type(
    effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::CRIPPLED:
            return 1;
        case effect::effect_t::SYMBOLIC_AVENGER:
            return 5;
        case effect::effect_t::MIGHT:
        case effect::effect_t::VULNERABILITY:
            return 25;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1500;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_effect_duration(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::CRIPPLED:
            return 10;
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            return 30;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return 1'000'000'000;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline int get_effective_effect_duration(
    int base_duration,
    effect::effect_t effect_type,
    const component::effective_attributes& effective_attributes,
    const effect_duration_pct_addends_t& effect_duration_pct_addends) {
    double effective_uniform_condition_duration_pct =
        effective_attributes[actor::attribute_t::CONDITION_DURATION_PCT] +
        effect_duration_pct_addends.condition_duration_pct_addend;
    double effective_uniform_boon_duration_pct =
        effective_attributes[actor::attribute_t::BOON_DURATION_PCT] +
        effect_duration_pct_addends.boon_duration_pct_addend;
    auto calculate_condition_duration = [&](double special_condition_duration_pct = 0.0) {
        double normalized_uniform_condition_duration_pct = std::min(
            100.0,
            std::max(effective_uniform_condition_duration_pct, special_condition_duration_pct));
        return (int)((double)base_duration *
                     (1.0 + normalized_uniform_condition_duration_pct / 100.0));
    };
    auto calculate_boon_duration = [&](double special_boon_duration_pct = 0.0) {
        double normalized_uniform_boon_duration_pct = std::min(
            100.0, std::max(effective_uniform_boon_duration_pct, special_boon_duration_pct));
        return (int)((double)base_duration * (1.0 + normalized_uniform_boon_duration_pct / 100.0));
    };

    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            // TODO: Implement boon-specific boon durations later
            return calculate_boon_duration();

        case effect::effect_t::CRIPPLED:
        case effect::effect_t::VULNERABILITY:
            return calculate_condition_duration();

        case effect::effect_t::BURNING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BURNING_DURATION_PCT] +
                effect_duration_pct_addends.burning_duration_pct_addend);
        case effect::effect_t::BLEEDING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BLEEDING_DURATION_PCT] +
                effect_duration_pct_addends.bleeding_duration_pct_addend);
        case effect::effect_t::TORMENT:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::TORMENT_DURATION_PCT] +
                effect_duration_pct_addends.torment_duration_pct_addend);
        case effect::effect_t::POISON:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::POISON_DURATION_PCT] +
                effect_duration_pct_addends.poison_duration_pct_addend);
        case effect::effect_t::CONFUSION:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::CONFUSION_DURATION_PCT] +
                effect_duration_pct_addends.confusion_duration_pct_addend);

        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return base_duration;

        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_boon(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
            return true;
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::CRIPPLED:
        case effect::effect_t::BINDING_BLADE:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return false;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_damaging_condition(effect::effect_t effect_type) {
    switch (effect_type) {
        case effect::effect_t::BURNING:
        case effect::effect_t::BLEEDING:
        case effect::effect_t::TORMENT:
        case effect::effect_t::POISON:
        case effect::effect_t::CONFUSION:
        case effect::effect_t::BINDING_BLADE:
            return true;
        case effect::effect_t::AEGIS:
        case effect::effect_t::ALACRITY:
        case effect::effect_t::FURY:
        case effect::effect_t::MIGHT:
        case effect::effect_t::QUICKNESS:
        case effect::effect_t::RESOLUTION:
        case effect::effect_t::RESISTANCE:
        case effect::effect_t::PROTECTION:
        case effect::effect_t::REGENERATION:
        case effect::effect_t::VIGOR:
        case effect::effect_t::SWIFTNESS:
        case effect::effect_t::STABILITY:
        case effect::effect_t::VULNERABILITY:
        case effect::effect_t::CRIPPLED:
        case effect::effect_t::VIRTUE_OF_JUSTICE:
        case effect::effect_t::SYMBOLIC_AVENGER:
        case effect::effect_t::INSPIRING_VIRTUE:
        case effect::effect_t::ASHES_OF_THE_JUST:
        case effect::effect_t::ARBITRARY_EFFECT:
            return false;
        case effect::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_EFFECT_UTILITIES_HPP
