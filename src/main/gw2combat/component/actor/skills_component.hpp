#ifndef GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/actor/skill.hpp"

namespace gw2combat::component {

struct skill_entity {
    actor::skill_t skill;
    entity_t entity;
};

struct skills_component {
    [[nodiscard]] inline std::vector<entity_t> find_by(const actor::skill_t& skill) const {
        std::vector<entity_t> entities;
        for (auto& skill_entity : skill_entities) {
            if (skill_entity.skill == skill) {
                entities.emplace_back(skill_entity.entity);
            }
        }
        return entities;
    }

    std::vector<skill_entity> skill_entities;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP
