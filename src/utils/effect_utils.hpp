#ifndef GW2COMBAT_UTILITIES_EFFECT_UTILS_HPP
#define GW2COMBAT_UTILITIES_EFFECT_UTILS_HPP

#include "basic_utils.hpp"

#include "actor/effect.hpp"
#include "actor/stacking.hpp"

#include "component/actor/effective_attributes.hpp"

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
    actor::effect_t::AEGIS,
    actor::effect_t::ALACRITY,
    actor::effect_t::FURY,
    actor::effect_t::MIGHT,
    actor::effect_t::QUICKNESS,
    actor::effect_t::RESOLUTION,
    actor::effect_t::RESISTANCE,
    actor::effect_t::PROTECTION,
    actor::effect_t::REGENERATION,
    actor::effect_t::VIGOR,
    actor::effect_t::SWIFTNESS,
    actor::effect_t::STABILITY,
};

[[nodiscard]] static inline actor::stacking_t get_effect_stacking_type(
    actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
            return actor::stacking_t::STACKING_DURATION;
        case actor::effect_t::MIGHT:
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
        case actor::effect_t::VULNERABILITY:
            return actor::stacking_t::STACKING_INTENSITY;
        case actor::effect_t::BINDING_BLADE:
            return actor::stacking_t::REPLACE;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_stored_stacks_of_effect_type(
    actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
        case actor::effect_t::BINDING_BLADE:
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
            return 1;
        case actor::effect_t::MIGHT:
        case actor::effect_t::VULNERABILITY:
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
            return 1500;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_considered_stacks_of_effect_type(
    actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
        case actor::effect_t::BINDING_BLADE:
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
            return 1;
        case actor::effect_t::MIGHT:
        case actor::effect_t::VULNERABILITY:
            return 25;
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
            return 1500;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline size_t get_max_effect_duration(actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
            return 10;
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::MIGHT:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
            return 30;
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
        case actor::effect_t::VULNERABILITY:
        case actor::effect_t::BINDING_BLADE:
            return 1'000'000'000;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] static inline int get_effective_effect_duration(
    int base_duration,
    actor::effect_t effect_type,
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
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::MIGHT:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
            // TODO: Implement boon-specific boon durations later
            return calculate_boon_duration();

        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
        case actor::effect_t::VULNERABILITY:
            // TODO: Implement non-damaging-condition-specific boon durations later
            return calculate_condition_duration();

        case actor::effect_t::BURNING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BURNING_DURATION_PCT] +
                effect_duration_pct_addends.burning_duration_pct_addend);
        case actor::effect_t::BLEEDING:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::BLEEDING_DURATION_PCT] +
                effect_duration_pct_addends.bleeding_duration_pct_addend);
        case actor::effect_t::TORMENT:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::TORMENT_DURATION_PCT] +
                effect_duration_pct_addends.torment_duration_pct_addend);
        case actor::effect_t::POISON:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::POISON_DURATION_PCT] +
                effect_duration_pct_addends.poison_duration_pct_addend);
        case actor::effect_t::CONFUSION:
            return calculate_condition_duration(
                effective_attributes[actor::attribute_t::CONFUSION_DURATION_PCT] +
                effect_duration_pct_addends.confusion_duration_pct_addend);

        case actor::effect_t::BINDING_BLADE:
            return base_duration;

        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_boon(actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::MIGHT:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
            return true;
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
        case actor::effect_t::VULNERABILITY:
        case actor::effect_t::BINDING_BLADE:
            return false;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

[[nodiscard]] constexpr static inline bool is_damaging_condition(actor::effect_t effect_type) {
    switch (effect_type) {
        case actor::effect_t::BURNING:
        case actor::effect_t::BLEEDING:
        case actor::effect_t::TORMENT:
        case actor::effect_t::POISON:
        case actor::effect_t::CONFUSION:
        case actor::effect_t::BINDING_BLADE:
            return true;
        case actor::effect_t::AEGIS:
        case actor::effect_t::ALACRITY:
        case actor::effect_t::FURY:
        case actor::effect_t::MIGHT:
        case actor::effect_t::QUICKNESS:
        case actor::effect_t::RESOLUTION:
        case actor::effect_t::RESISTANCE:
        case actor::effect_t::PROTECTION:
        case actor::effect_t::REGENERATION:
        case actor::effect_t::VIGOR:
        case actor::effect_t::SWIFTNESS:
        case actor::effect_t::STABILITY:
        case actor::effect_t::BLINDED:
        case actor::effect_t::CHILLED:
        case actor::effect_t::CRIPPLED:
        case actor::effect_t::FEAR:
        case actor::effect_t::IMMOBILIZED:
        case actor::effect_t::SLOW:
        case actor::effect_t::TAUNT:
        case actor::effect_t::WEAKNESS:
        case actor::effect_t::VULNERABILITY:
            return false;
        case actor::effect_t::INVALID:
            throw std::runtime_error("cannot deal with invalid effects!");
    }
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_EFFECT_UTILS_HPP
