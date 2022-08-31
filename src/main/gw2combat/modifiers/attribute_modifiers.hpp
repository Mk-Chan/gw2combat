#ifndef GW2COMBAT_MODIFIERS_ATTRIBUTE_MODIFIERS_HPP
#define GW2COMBAT_MODIFIERS_ATTRIBUTE_MODIFIERS_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::modifiers {

struct attribute_modifiers_t {
    [[nodiscard]] constexpr inline bool operator<=>(
        const attribute_modifiers_t& attribute_modifiers) const = default;

    double power_addend = 0.0;
    double precision_addend = 0.0;
    double toughness_addend = 0.0;
    double vitality_addend = 0.0;
    double concentration_addend = 0.0;
    double condition_damage_addend = 0.0;
    double expertise_addend = 0.0;
    double ferocity_addend = 0.0;
    double healing_power_addend = 0.0;
    double armor_addend = 0.0;
    double max_health_addend = 0.0;

    double boon_duration_pct_addend = 0.0;
    double critical_chance_pct_addend = 0.0;
    double critical_damage_pct_addend = 0.0;
    double condition_duration_pct_addend = 0.0;
    double burning_duration_pct_addend = 0.0;
    double bleeding_duration_pct_addend = 0.0;
    double confusion_duration_pct_addend = 0.0;
    double poison_duration_pct_addend = 0.0;
    double torment_duration_pct_addend = 0.0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modifiers_t,
                                                power_addend,
                                                precision_addend,
                                                toughness_addend,
                                                vitality_addend,
                                                concentration_addend,
                                                condition_damage_addend,
                                                expertise_addend,
                                                ferocity_addend,
                                                healing_power_addend,
                                                armor_addend,
                                                max_health_addend,

                                                boon_duration_pct_addend,
                                                critical_chance_pct_addend,
                                                critical_damage_pct_addend,
                                                condition_duration_pct_addend,
                                                burning_duration_pct_addend,
                                                bleeding_duration_pct_addend,
                                                confusion_duration_pct_addend,
                                                poison_duration_pct_addend,
                                                torment_duration_pct_addend)

}  // namespace gw2combat::modifiers

#endif  // GW2COMBAT_MODIFIERS_ATTRIBUTE_MODIFIERS_HPP
