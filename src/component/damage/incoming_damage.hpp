#ifndef GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_HPP

#include "common.hpp"

#include "actor/effect.hpp"
#include "actor/skill.hpp"

namespace gw2combat::component {

struct incoming_damage_event {
    tick_t tick = 0;
    entity_t source_entity = entt::tombstone;
    actor::effect_t effect = actor::effect_t::INVALID;
    actor::skill_t skill;
    double value = 0.0;
};

struct incoming_damage {
    std::vector<incoming_damage_event> incoming_damage_events;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_damage_event,
                                                tick,
                                                source_entity,
                                                effect,
                                                skill,
                                                value)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(incoming_damage, incoming_damage_events)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_INCOMING_DAMAGE_HPP
