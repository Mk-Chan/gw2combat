#ifndef GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP

namespace gw2combat::component {

struct buffered_condition_damage {
    double value = 0.0;
    double unaffected_by_incoming_multiplier_value = 0.0;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
