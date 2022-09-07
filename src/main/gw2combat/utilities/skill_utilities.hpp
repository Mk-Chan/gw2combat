#ifndef GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP

#include "base_utilities.hpp"

#include "entity_utilities.hpp"

#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
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
    bool has_bundle_equipped = registry.any_of<component::bundle_component>(actor_entity);
    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (!has_bundle_equipped && skill_ammo.current_ammo <= 0) {
        return false;
    }

    auto& skill_configuration =
        registry.get<actor::skill_database>(utils::get_owner(actor_entity, registry))
            .find_by(skill);
    if (skill_configuration.is_child_skill &&
        !registry.any_of<component::owner_actor>(actor_entity)) {
        return false;
    }
    if (!skill_configuration.is_child_skill && !has_bundle_equipped &&
        skill_configuration.weapon_type != actor::weapon_type::EMPTY_HANDED &&
        skill_configuration.weapon_type != actor::weapon_type::INVALID) {
        auto& weapons = registry.get<component::equipped_weapons>(actor_entity).weapons;
        auto current_weapon_set = registry.get<component::current_weapon_set>(actor_entity).set;
        return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
            return weapon.set == current_weapon_set &&
                   weapon.type == skill_configuration.weapon_type &&
                   (skill_configuration.weapon_position == actor::weapon_position::UNIVERSAL ||
                    skill_configuration.weapon_position == weapon.position);
        });
    }

    return true;
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

static inline void reset_skill_cooldown(entity_t actor_entity,
                                        const actor::skill_t& skill,
                                        registry_t& registry) {
    auto& skills_component = registry.get<component::skills_component>(actor_entity);
    if (skills_component.has(skill)) {
        auto skill_entity = skills_component.find_by(skill);
        if (registry.any_of<component::recharge>(skill_entity)) {
            auto& recharge = registry.get<component::recharge>(skill_entity);
            recharge.progress = recharge.duration;
        }
        if (registry.any_of<component::cooldown>(skill_entity)) {
            auto& cooldown = registry.get<component::cooldown>(skill_entity);
            cooldown.progress = cooldown.duration;
        }
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
