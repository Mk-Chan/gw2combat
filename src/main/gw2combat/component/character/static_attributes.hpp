#ifndef GW2COMBAT_COMPONENT_STATIC_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_STATIC_ATTRIBUTES_HPP

namespace gw2combat::component {

// https://wiki.guildwars2.com/wiki/Attribute
struct static_attributes {
    // Primary attributes
    unsigned int power;
    unsigned int precision;
    unsigned int toughness;
    unsigned int vitality;

    // Secondary attributes
    unsigned int concentration;
    unsigned int condition_damage;
    unsigned int expertise;
    unsigned int ferocity;
    unsigned int healing_power;

    // Derived attributes
    unsigned int armor;
    double boon_duration_pct;
    double critical_chance_pct;
    double critical_damage_pct;
    double condition_duration_pct;
    unsigned int max_health = 1;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_STATIC_ATTRIBUTES_HPP
