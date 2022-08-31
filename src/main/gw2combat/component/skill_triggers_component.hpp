#ifndef GW2COMBAT_COMPONENT_SKILL_TRIGGERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILL_TRIGGERS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "filters.hpp"

namespace gw2combat::component {

struct skill_triggers_t {
    filters_t filters;

    actor::skill_t skill;
};

// This component is applied on traits/effects with owner_actor
struct skill_triggers_component {
    std::vector<skill_triggers_t> skill_triggers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_triggers_t, filters, skill)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_triggers_component, skill_triggers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_TRIGGERS_COMPONENT_HPP
