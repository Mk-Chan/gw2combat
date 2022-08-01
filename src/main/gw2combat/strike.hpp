#ifndef GW2COMBAT_STRIKE_DAMAGE_HPP
#define GW2COMBAT_STRIKE_DAMAGE_HPP

#include "gw2combat/skills.hpp"

namespace gw2combat {

struct strike {
    entity_t source;
    double outgoing_strike_damage_multiplier;
    double critical_hit_multiplier;
    skills::skill skill;
};

}  // namespace gw2combat

#endif  // GW2COMBAT_STRIKE_DAMAGE_HPP
