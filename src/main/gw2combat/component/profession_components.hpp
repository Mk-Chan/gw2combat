#ifndef GW2COMBAT_COMPONENT_PROFESSION_PROFESSION_COMPONENTS_HPP
#define GW2COMBAT_COMPONENT_PROFESSION_PROFESSION_COMPONENTS_HPP

namespace gw2combat::component {

struct virtue_of_justice {
    unsigned int number_of_ticks_for_burning_application{5};
    unsigned int num_unaccounted_hits{0};
    tick_t cooldown{0};
    tick_t cooldown_progress{0};
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_PROFESSION_PROFESSION_COMPONENTS_HPP
