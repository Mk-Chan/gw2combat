#ifndef GW2COMBAT_COMPONENT_DYNAMIC_ATTRIBUTES_HPP
#define GW2COMBAT_COMPONENT_DYNAMIC_ATTRIBUTES_HPP

namespace gw2combat::component {

struct dynamic_attributes {
    unsigned int max_endurance = 100;
    double endurance_gain_pct = 0;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DYNAMIC_ATTRIBUTES_HPP
