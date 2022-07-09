#ifndef GW2COMBAT_COMPONENT_VIRTUE_OF_JUSTICE_HPP
#define GW2COMBAT_COMPONENT_VIRTUE_OF_JUSTICE_HPP

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct virtue_of_justice {
    explicit virtue_of_justice(unsigned int number_of_ticks_for_burning_application)
        : number_of_ticks_for_burning_application(number_of_ticks_for_burning_application),
          next_burning_application_hit_count(number_of_ticks_for_burning_application) {
    }

    unsigned int number_of_ticks_for_burning_application;
    unsigned int next_burning_application_hit_count;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_VIRTUE_OF_JUSTICE_HPP
