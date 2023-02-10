#ifndef GW2COMBAT_COMPONENT_EFFECT_IS_UNIQUE_EFFECT_HPP
#define GW2COMBAT_COMPONENT_EFFECT_IS_UNIQUE_EFFECT_HPP

#include "actor/unique_effect.hpp"

namespace gw2combat::component {

struct is_unique_effect {
    actor::unique_effect_t unique_effect;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECT_IS_UNIQUE_EFFECT_HPP
