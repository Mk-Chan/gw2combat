#ifndef GW2COMBAT_COMPONENT_TRAITS_TRAIT_COMPONENTS_HPP
#define GW2COMBAT_COMPONENT_TRAITS_TRAIT_COMPONENTS_HPP

namespace gw2combat::component {

struct fiery_wrath {
    constexpr static double strike_damage_increase_if_target_burning = 0.07;
};
struct inspired_virtue {
    constexpr static double strike_damage_increase_per_boon = 0.01;
};
struct retribution {
    constexpr static double strike_damage_increase_if_has_resolution = 0.1;
};
struct symbolic_avenger_trait {
    constexpr static double strike_damage_increase_per_stack = 0.02;
};
struct symbolic_exposure {
    constexpr static double strike_damage_increase_if_target_vulnerable = 0.05;
};
struct symbolic_power {
    constexpr static inline double symbol_strike_damage_increase = 0.3;
};
struct unscathed_contender {
    constexpr static double strike_damage_increase_if_has_aegis = 0.2;
};

}

#endif  // GW2COMBAT_COMPONENT_TRAITS_TRAIT_COMPONENTS_HPP
