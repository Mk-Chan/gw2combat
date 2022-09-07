#ifndef GW2COMBAT_COMPONENT_SKILL_MODIFIERS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILL_MODIFIERS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "filters.hpp"

#include "gw2combat/effect/application.hpp"

namespace gw2combat::component {

struct skill_modifiers_t {
    filters_t filters;

    double cooldown_reduction_pct = 0;
    std::vector<int> duration_increase_ms;
    std::vector<effect::application_t> on_strike_effect_applications;
};

// This component is applied on traits/effects with owner_actor
struct skill_modifiers_component {
    std::vector<skill_modifiers_t> skill_modifiers;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_modifiers_t,
                                                filters,

                                                cooldown_reduction_pct,
                                                duration_increase_ms,
                                                on_strike_effect_applications)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_modifiers_component, skill_modifiers)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILL_MODIFIERS_COMPONENT_HPP
