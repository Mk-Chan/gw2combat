#ifndef GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP
#define GW2COMBAT_COMPONENT_SKILLS_COMPONENT_HPP

#include "common.hpp"

#include "actor/skill.hpp"

#include "utils/io_utils.hpp"

namespace gw2combat::component {

struct skill_entity {
    actor::skill_t skill;
    entity_t entity;
};

struct skills_component {
    [[nodiscard]] inline bool has(const actor::skill_t& skill) const {
        return std::any_of(
            skill_entities.begin(), skill_entities.end(), [&](const skill_entity& skill_entity) {
                return skill == skill_entity.skill;
            });
    }
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
