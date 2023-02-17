#ifndef GW2COMBAT_COMPONENT_ACTOR_CASTING_SKILL_HPP
#define GW2COMBAT_COMPONENT_ACTOR_CASTING_SKILL_HPP

#include "common.hpp"

namespace gw2combat::component {

struct casting_skill {
    entity_t skill_entity = entt::tombstone;
    int next_strike_idx = 0;
    int next_pulse_idx = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(casting_skill,
                                                skill_entity,
                                                next_strike_idx,
                                                next_pulse_idx)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_CASTING_SKILL_HPP
