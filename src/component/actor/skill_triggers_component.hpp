#ifndef GW2COMBAT_COMPONENT_ACTOR_SKILL_TRIGGERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_ACTOR_SKILL_TRIGGERS_COMPONENT_HPP

#include "configuration/skill_trigger.hpp"

namespace gw2combat::component {

struct skill_triggers_component {
    std::vector<configuration::skill_trigger_t> skill_triggers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_triggers_component, skill_triggers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_ACTOR_SKILL_TRIGGERS_COMPONENT_HPP
