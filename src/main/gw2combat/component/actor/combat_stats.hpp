#ifndef GW2COMBAT_COMPONENT_ACTOR_COMBAT_STATS_HPP
#define GW2COMBAT_COMPONENT_ACTOR_COMBAT_STATS_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::component {

struct combat_stats {
    int health;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(combat_stats, health)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_COMBAT_STATS_HPP