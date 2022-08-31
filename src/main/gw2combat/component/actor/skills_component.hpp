#ifndef GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include "gw2combat/actor/skill.hpp"

namespace gw2combat::component {

struct skill_entity {
    actor::skill_t skill;
    entity_t entity;
};

struct skills_component {
    [[nodiscard]] inline entity_t find_by(const actor::skill_t& skill) const {
        for (auto& skill_entity : skill_entities) {
            if (skill_entity.skill == skill) {
                return skill_entity.entity;
            }
        }
        throw std::runtime_error(
            fmt::format("skill with key {} not found!", utils::to_string(skill)));
    }

    std::vector<skill_entity> skill_entities;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP
