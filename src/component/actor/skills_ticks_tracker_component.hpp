#ifndef GW2COMBAT_COMPONENT_SKILLS_TICKS_TRACKER_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_TICKS_TRACKER_COMPONENT_HPP

#include "common.hpp"

namespace gw2combat::component {

struct skills_ticks_tracker_component {
    struct skill_state_t {
        entity_t skill_entity = entt::tombstone;
        tick_t skill_tick_progress = 0;
        int next_skill_tick_idx = 0;
        std::map<int, double> weapon_strength_roll_by_group{};
    };

    std::vector<skill_state_t> skills;
};

struct destroy_skills_ticks_tracker_component {
    std::vector<entity_t> skill_entities;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_ticks_tracker_component::skill_state_t,
                                                skill_entity,
                                                skill_tick_progress,
                                                next_skill_tick_idx,
                                                weapon_strength_roll_by_group)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skills_ticks_tracker_component, skills)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(destroy_skills_ticks_tracker_component,
                                                skill_entities)

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_TICKS_TRACKER_COMPONENT_HPP
