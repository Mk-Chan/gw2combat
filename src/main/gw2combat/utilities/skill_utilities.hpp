#ifndef GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/skill/ammo.hpp"
#include "gw2combat/component/skill/recharge.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool can_cast_skill(entity_t actor_entity,
                                                const actor::skill_t& skill,
                                                registry_t& registry) {
    auto skill_entity = registry.get<component::skills_component>(actor_entity).find_by(skill);
    if (registry.any_of<component::recharge>(skill_entity)) {
        return false;
    }
    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    return skill_ammo.current_ammo > 0;
}

static inline void put_skill_on_cooldown(entity_t actor_entity,
                                         const actor::skill_t& skill,
                                         registry_t& registry) {
    auto skill_entity = registry.get<component::skills_component>(actor_entity).find_by(skill);

    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0) {
        throw std::runtime_error(fmt::format("actor {} skill {} doesn't have any more ammo",
                                             get_entity_name(actor_entity, registry),
                                             to_string(skill)));
    }

    --skill_ammo.current_ammo;

    auto& skill_configuration =
        registry.get<actor::skill_database>(utils::get_owner(actor_entity, registry))
            .find_by(skill);
    registry.emplace<component::recharge>(
        skill_entity, component::recharge{skill_configuration.recharge_duration});
    if (!registry.any_of<component::cooldown>(skill_entity)) {
        registry.emplace<component::cooldown>(skill_entity,
                                              component::cooldown{skill_configuration.cooldown});
    }
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
