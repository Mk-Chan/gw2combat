#ifndef GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct skills_actions_component {
    struct skill_state_t {
        entity_t skill_entity = entt::tombstone;
        std::array<int, 2> action_progress{0, 0};
        int next_strike_idx = 0;
        int next_pulse_idx = 0;
        // int next_blast_idx = 0;
        // int next_leap_idx = 0;
        // int next_projectile_idx = 0;
        int next_whirl_idx = 0;
        double weapon_strength_roll = 0.0;
    };

    std::vector<skill_state_t> skills;
};

struct finished_skills_actions_component {
    std::vector<entity_t> skill_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_actions_component::skill_state_t,
                                                skill_entity,
                                                action_progress,
                                                next_strike_idx,
                                                next_pulse_idx,
                                                // next_blast_idx,
                                                // next_leap_idx,
                                                // next_projectile_idx,
                                                next_whirl_idx,
                                                weapon_strength_roll)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_actions_component, skills)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(finished_skills_actions_component, skill_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_ACTIONS_COMPONENT_HPP
