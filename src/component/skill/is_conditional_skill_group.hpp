#ifndef GW2COMBAT_COMPONENT_SKILL_IS_CONDITIONAL_SKILL_GROUP_HPP
#define GW2COMBAT_COMPONENT_SKILL_IS_CONDITIONAL_SKILL_GROUP_HPP

#include "configuration/skill.hpp"

namespace gw2combat::component {

struct is_conditional_skill_group {
    configuration::conditional_skill_group_t conditional_skill_group_configuration;
};

struct is_part_of_conditional_skill_group {
    entity_t conditional_skill_group_entity;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_IS_CONDITIONAL_SKILL_GROUP_HPP
