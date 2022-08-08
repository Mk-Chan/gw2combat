#ifndef GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP

#include "gw2combat/effects.hpp"

namespace gw2combat::component {

struct buffered_condition_damage {
    struct damage_unit {
        effects::effect_type effect_type;
        entity_t source;
        double damage;
    };
    std::vector<damage_unit> damage_units;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
