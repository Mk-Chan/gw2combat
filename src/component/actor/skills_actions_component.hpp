#ifndef GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct skills_actions_component {
    struct skill_state_t {
        entity_t skill_entity = entt::tombstone;
        std::array<int, 2> pulse_progress{0, 0};
        std::array<int, 2> pulse_duration{0, 0};
        std::array<int, 2> strike_progress{0, 0};
        std::array<int, 2> strike_duration{0, 0};
        int next_strike_idx = 0;
        int next_pulse_idx = 0;
    };

    std::vector<skill_state_t> skills;
};

struct finished_skills_actions_component {
    std::vector<entity_t> skill_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_actions_component::skill_state_t,
                                                skill_entity,
                                                pulse_progress,
                                                pulse_duration,
                                                strike_progress,
                                                strike_duration,
                                                next_strike_idx,
                                                next_pulse_idx)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_actions_component, skills)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(finished_skills_actions_component, skill_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP
