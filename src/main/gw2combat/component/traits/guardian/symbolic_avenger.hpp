#ifndef GW2COMBAT_COMPONENT_TRAITS_GUARDIAN_SYMBOLIC_AVENGER_HPP
#define GW2COMBAT_COMPONENT_TRAITS_GUARDIAN_SYMBOLIC_AVENGER_HPP

namespace gw2combat::component {

struct symbolic_avenger {
    constexpr static double strike_damage_increase_per_stack = 0.02;

    std::vector<effect> stacks;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_TRAITS_GUARDIAN_SYMBOLIC_AVENGER_HPP
