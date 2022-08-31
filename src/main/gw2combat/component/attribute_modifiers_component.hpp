#ifndef GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFIERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFIERS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/actor/attributes.hpp"

#include "gw2combat/component/filters.hpp"

namespace gw2combat::component {

struct attribute_modifiers_t {
    filters_t filters;

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

    [[nodiscard]] inline bool operator==(const attribute_modifiers_t& rhs) const {
        return filters == rhs.filters && power_addend == rhs.power_addend &&
               precision_addend == rhs.precision_addend &&
               toughness_addend == rhs.toughness_addend && vitality_addend == rhs.vitality_addend &&
               concentration_addend == rhs.concentration_addend &&
               condition_damage_addend == rhs.condition_damage_addend &&
               expertise_addend == rhs.expertise_addend && ferocity_addend == rhs.ferocity_addend &&
               healing_power_addend == rhs.healing_power_addend &&
               armor_addend == rhs.armor_addend && max_health_addend == rhs.max_health_addend &&
               boon_duration_pct_addend == rhs.boon_duration_pct_addend &&
               critical_chance_pct_addend == rhs.critical_chance_pct_addend &&
               critical_damage_pct_addend == rhs.critical_damage_pct_addend &&
               condition_duration_pct_addend == rhs.condition_duration_pct_addend &&
               burning_duration_pct_addend == rhs.burning_duration_pct_addend &&
               bleeding_duration_pct_addend == rhs.bleeding_duration_pct_addend &&
               confusion_duration_pct_addend == rhs.confusion_duration_pct_addend &&
               poison_duration_pct_addend == rhs.poison_duration_pct_addend &&
               torment_duration_pct_addend == rhs.torment_duration_pct_addend;
    }
};

// This component is applied on skills/traits/effects with owner_actor
struct attribute_modifiers_component {
    std::vector<attribute_modifiers_t> attribute_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modifiers_t,
                                                filters,

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(attribute_modifiers_component, attribute_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ATTRIBUTE_MODIFIERS_COMPONENT_HPP
