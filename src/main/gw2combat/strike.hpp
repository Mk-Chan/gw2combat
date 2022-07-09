#ifndef GW2COMBAT_STRIKE_DAMAGE_HPP
#define GW2COMBAT_STRIKE_DAMAGE_HPP

#include <entt/entt.hpp>

#include "types.hpp"

namespace gw2combat {

struct strike {
    entt::entity source;
    double damage;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_STRIKE_DAMAGE_HPP
