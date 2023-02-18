#ifndef GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP
#define GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP

#include "actor/skill.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct skill_trigger_t {
    condition_t condition;

    actor::skill_t skill_key;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_trigger_t, condition, skill_key)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_SKILL_TRIGGER_HPP
