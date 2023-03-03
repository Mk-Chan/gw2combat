#ifndef GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
#define GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP

#include "common.hpp"

#include "actor/effect.hpp"
#include "actor/skill.hpp"

namespace gw2combat::component {

struct condition_damage_t {
    entity_t effect_source_entity;
    actor::effect_t effect;
    actor::skill_t source_skill;
    double damage;
};

struct buffered_condition_damage {
    std::vector<condition_damage_t> condition_damage_buffer;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_DAMAGE_BUFFERED_CONDITION_DAMAGE_HPP
