#ifndef GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/cooldown.hpp"

#include "gw2combat/actor/skill.hpp"
#include "gw2combat/component/skill/recharge.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool skill_has_tag(const actor::skill_t& skill,
                                               actor::skill_tag_t skill_tag) {
    auto& skill_tags = actor::skill_database::instance().find_by(skill).tags;
    return std::find_if(skill_tags.cbegin(),
                        skill_tags.cend(),
                        [&](const actor::skill_tag_t& current_skill_tag) {
                            return current_skill_tag == skill_tag;
                        }) != skill_tags.cend();
}

[[nodiscard]] static inline bool skill_has_cooldown(entity_t actor_entity,
                                                    const actor::skill_t& skill,
                                                    registry_t& registry) {
    auto entities = registry.get<component::skills_component>(actor_entity).find_by(skill);
    return std::all_of(entities.begin(), entities.end(), [&](entity_t entity) {
        return registry.any_of<component::cooldown>(entity);
    });
}

[[nodiscard]] static inline bool skill_has_recharge(entity_t actor_entity,
                                                    const actor::skill_t& skill,
                                                    registry_t& registry) {
    auto entities = registry.get<component::skills_component>(actor_entity).find_by(skill);
    return std::all_of(entities.begin(), entities.end(), [&](entity_t entity) {
        return registry.any_of<component::recharge>(entity);
    });
}

static inline void put_skill_on_cooldown(entity_t actor_entity,
                                         const actor::skill_t& skill,
                                         registry_t& registry) {
    auto entities = registry.get<component::skills_component>(actor_entity).find_by(skill);
    for (auto entity : entities) {
        if (!registry.any_of<component::cooldown>(entity)) {
            registry.emplace<component::cooldown>(
                entity,
                component::cooldown{actor::skill_database::instance().find_by(skill).cooldown});
            return;
        }
    }
    throw std::runtime_error(
        fmt::format("all entities of skill {} already on cooldown", utils::to_string(skill)));
}

[[nodiscard]] static inline bool actor_has_skill(entity_t actor_entity,
                                                 const actor::skill_t& skill,
                                                 registry_t& registry) {
    if (!registry.any_of<component::skills_component>(actor_entity)) {
        throw std::runtime_error(fmt::format("actor {} doesn't have skills component",
                                             get_entity_name(actor_entity, registry)));
    }
    auto& skill_entities = registry.get<component::skills_component>(actor_entity).skill_entities;
    return std::find_if(skill_entities.cbegin(),
                        skill_entities.cend(),
                        [&](const component::skill_entity& current_skill_entity) {
                            return skill == current_skill_entity.skill;
                        }) != skill_entities.cend();
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
