#ifndef GW2COMBAT_COMPONENT_GEAR_SIGIL_SIGIL_IMPACT_HPP
#define GW2COMBAT_COMPONENT_GEAR_SIGIL_SIGIL_IMPACT_HPP

namespace gw2combat::component {

struct sigil_impact {
    constexpr static double strike_damage_increase = 0.03;
    constexpr static double strike_damage_increase_if_target_stunned = 0.07;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_GEAR_SIGIL_SIGIL_IMPACT_HPP
