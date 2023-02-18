#ifndef GW2COMBAT_UTILS_SKILL_UTILS_HPP
#define GW2COMBAT_UTILS_SKILL_UTILS_HPP

#include "basic_utils.hpp"

#include "entity_utils.hpp"

#include "component/actor/skills_component.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/skill_configuration_component.hpp"
#include "component/temporal/cooldown_component.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline configuration::skill_t
get_skill_configuration(const actor::skill_t& skill, entity_t actor_entity, registry_t& registry) {
    auto& skill_component = registry.get<component::skills_component>(actor_entity);
    return registry.get<component::skill_configuration_component>(skill_component.find_by(skill))
        .skill_configuration;
}

static inline void put_skill_on_cooldown(entity_t actor_entity,
                                         const actor::skill_t& skill,
                                         registry_t& registry) {
    auto skill_entity = registry.get<component::skills_component>(actor_entity).find_by(skill);

    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0) {
        utils::log_component<component::cooldown_component>(registry);
        throw std::runtime_error(fmt::format("actor {} skill {} doesn't have any more ammo",
                                             get_entity_name(actor_entity, registry),
                                             to_string(skill)));
    }
    --skill_ammo.current_ammo;

    auto& skill_component = registry.get<component::skills_component>(actor_entity);
    auto& skill_configuration =
        registry.get<component::skill_configuration_component>(skill_component.find_by(skill))
            .skill_configuration;
    if (!registry.any_of<component::cooldown_component>(skill_entity)) {
        registry.emplace<component::cooldown_component>(
            skill_entity, component::cooldown_component{skill_configuration.cooldown});
    }
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_SKILL_UTILS_HPP
