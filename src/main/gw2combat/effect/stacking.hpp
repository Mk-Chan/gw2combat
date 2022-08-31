#ifndef GW2COMBAT_EFFECT_STACKING_HPP
#define GW2COMBAT_EFFECT_STACKING_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::effect {

enum class stacking_t : std::uint8_t
{
    STACKING_INTENSITY,
    STACKING_DURATION,
};

}  // namespace gw2combat::effect

#endif  // GW2COMBAT_EFFECT_STACKING_HPP
