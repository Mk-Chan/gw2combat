#ifndef GW2COMBAT_UTILS_SKILL_UTILS_HPP
#define GW2COMBAT_UTILS_SKILL_UTILS_HPP

#include "basic_utils.hpp"
#include "entity_utils.hpp"
#include "io_utils.hpp"

#include "component/actor/skills_component.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/skill_configuration_component.hpp"
#include "component/temporal/cooldown_component.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline configuration::skill_t&
get_skill_configuration(const actor::skill_t& skill, entity_t actor_entity, registry_t& registry) {
    auto& skill_component = registry.get<component::skills_component>(actor_entity);
    return registry.get<component::skill_configuration_component>(skill_component.find_by(skill))
        .skill_configuration;
}

static inline void assert_can_cast_skill(entity_t actor_entity,
                                         const actor::skill_t& skill,
                                         registry_t& registry) {
    if (registry.any_of<component::owner_component>(actor_entity)) {
        return;
    }

    auto skill_entity = registry.get<component::skills_component>(actor_entity).find_by(skill);
    // if (registry.any_of<component::internal_cooldown_component>(skill_entity)) {
    //     throw std::runtime_error(fmt::format("skill {} is on internal cooldown", skill));
    // }

    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0) {
        utils::log_component<component::cooldown_component>(registry);
        throw std::runtime_error(fmt::format("actor {} skill {} doesn't have any more ammo",
                                             get_entity_name(actor_entity, registry),
                                             to_string(skill)));
    }

    auto& skill_configuration = utils::get_skill_configuration(skill, actor_entity, registry);
    auto bundle_ptr = registry.try_get<component::bundle_component>(actor_entity);
    if (!bundle_ptr && skill_configuration.weapon_type != actor::weapon_type::EMPTY_HANDED &&
        skill_configuration.weapon_type != actor::weapon_type::INVALID &&
        skill_configuration.weapon_type != actor::weapon_type::MAIN_HAND) {
        auto& weapons = registry.get<component::equipped_weapons>(actor_entity).weapons;
        auto current_weapon_set = registry.get<component::current_weapon_set>(actor_entity).set;
        bool skill_available_on_weapon =
            std::any_of(weapons.begin(), weapons.end(), [&](const component::weapon_t& weapon) {
                return weapon.set == current_weapon_set &&
                       weapon.type == skill_configuration.weapon_type;
            });
        if (!skill_available_on_weapon) {
            throw std::runtime_error(
                fmt::format("skill {} not available on this weapon set", skill));
        }
    }
}

[[nodiscard]] static inline bool skill_has_tag(const configuration::skill_t& skill,
                                               const actor::skill_tag_t& skill_tag) {
    return std::find(skill.tags.cbegin(), skill.tags.cend(), skill_tag) != skill.tags.cend();
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
