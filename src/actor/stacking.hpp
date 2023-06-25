#ifndef GW2COMBAT_ACTOR_STACKING_HPP
#define GW2COMBAT_ACTOR_STACKING_HPP

#include "common.hpp"

namespace gw2combat::actor {

enum class stacking_t : std::uint8_t
{
    STACKING_INTENSITY,
    STACKING_DURATION,
    REPLACE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(stacking_t,
                             {
                                 {stacking_t::STACKING_INTENSITY, "stacking_intensity"},
                                 {stacking_t::STACKING_INTENSITY, "intensity"},
                                 {stacking_t::STACKING_DURATION, "stacking_duration"},
                                 {stacking_t::STACKING_DURATION, "duration"},
                                 {stacking_t::REPLACE, "replace"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_STACKING_HPP
