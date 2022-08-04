#ifndef GW2COMBAT_COMPONENT_EFFECTIVE_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_EFFECTIVE_ATTRIBUTES_HPP

namespace gw2combat::component {

struct effective_attributes {
    // Primary attributes
    unsigned int power = 0;
    unsigned int precision = 0;
    unsigned int toughness = 0;
    unsigned int vitality = 0;

    // Secondary attributes
    unsigned int concentration = 0;
    unsigned int condition_damage = 0;
    unsigned int expertise = 0;
    unsigned int ferocity = 0;
    unsigned int healing_power = 0;

    // Derived attributes
    unsigned int armor = 0;
    double boon_duration_pct = 0.0;
    double critical_chance_pct = 0.0;
    double critical_damage_pct = 0.0;
    double condition_duration_pct = 0.0;
    double burning_duration_pct = condition_duration_pct;
    double bleeding_duration_pct = condition_duration_pct;
    double confusion_duration_pct = condition_duration_pct;
    double poison_duration_pct = condition_duration_pct;
    double torment_duration_pct = condition_duration_pct;

    unsigned int max_health = 1;

    // Dynamic attributes
    unsigned int endurance = 100;
    double endurance_gain_pct = 0;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECTIVE_ATTRIBUTES_HPP
