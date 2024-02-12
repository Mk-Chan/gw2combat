#ifndef GW2COMBAT_COMPONENT_EFFECT_IS_SKILL_TRIGGER_HPP
#define GW2COMBAT_COMPONENT_EFFECT_IS_SKILL_TRIGGER_HPP

#include "configuration/skill_trigger.hpp"

namespace gw2combat::component {

struct is_skill_trigger {
    configuration::skill_trigger_t skill_trigger;
    bool already_triggered = false;
};

struct is_unchained_skill_trigger {
    configuration::skill_trigger_t skill_trigger;
};

struct is_source_actor_skill_trigger {
    configuration::skill_trigger_t skill_trigger;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_EFFECT_IS_SKILL_TRIGGER_HPP
