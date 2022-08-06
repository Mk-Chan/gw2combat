#ifndef GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP

#include "gw2combat/effects.hpp"
#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct effects_component {
    std::multimap<effects::effect_type, effects::effect_stack> effects;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECTS_COMPONENT_HPP
