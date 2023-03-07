#ifndef GW2COMBAT_COMPONENT_SKILL_IS_SKILL_HPP
#define GW2COMBAT_COMPONENT_SKILL_IS_SKILL_HPP

#include "configuration/skill.hpp"

namespace gw2combat::component {

struct is_skill {
    configuration::skill_t skill_configuration;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_IS_SKILL_HPP
