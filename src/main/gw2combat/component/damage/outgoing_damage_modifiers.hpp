#ifndef GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_DAMAGE_MODIFIERS_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_DAMAGE_MODIFIERS_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct outgoing_damage_modifiers {
    double strike_damage_multiplier = 1.0;
    double strike_damage_multiplier_add_group_addend = 0.0;
    double condition_damage_multiplier = 1.0;
    double condition_damage_multiplier_add_group_addend = 0.0;
    double burning_damage_multiplier = 1.0;
    double bleeding_damage_multiplier = 1.0;
    double confusion_damage_multiplier = 1.0;
    double poison_damage_multiplier = 1.0;
    double torment_damage_multiplier = 1.0;

    double condition_duration_pct_addend = 0.0;
    double burning_duration_pct_addend = 0.0;
    double bleeding_duration_pct_addend = 0.0;
    double confusion_duration_pct_addend = 0.0;
    double poison_duration_pct_addend = 0.0;
    double torment_duration_pct_addend = 0.0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(outgoing_damage_modifiers,
                                                strike_damage_multiplier,
                                                strike_damage_multiplier_add_group_addend,
                                                condition_damage_multiplier,
                                                condition_damage_multiplier_add_group_addend,
                                                burning_damage_multiplier,
                                                bleeding_damage_multiplier,
                                                confusion_damage_multiplier,
                                                poison_damage_multiplier,
                                                torment_damage_multiplier,

                                                condition_duration_pct_addend,
                                                burning_duration_pct_addend,
                                                bleeding_duration_pct_addend,
                                                confusion_duration_pct_addend,
                                                poison_duration_pct_addend,
                                                torment_duration_pct_addend)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_OUTGOING_DAMAGE_MODIFIERS_HPP
