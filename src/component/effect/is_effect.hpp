#ifndef GW2COMBAT_COMPONENT_EFFECT_IS_EFFECT_HPP
#define GW2COMBAT_COMPONENT_EFFECT_IS_EFFECT_HPP

#include "actor/effect.hpp"

namespace gw2combat::component {

struct is_effect {
    actor::effect_t effect;
    int grouped_with_num_stacks;
};

struct is_damaging_effect {};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECT_IS_EFFECT_HPP
