#ifndef GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP

#include "gw2combat/effects.hpp"
#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct effects_component {
    std::multimap<effects::effect_type, effects::effect_stack> effects;
};

struct ashes_of_the_just_component {
    tick_t cooldown_progress = 1'000;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP
