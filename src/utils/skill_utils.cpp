#include "skill_utils.hpp"

#include "basic_utils.hpp"
#include "condition_utils.hpp"
#include "entity_utils.hpp"
#include "io_utils.hpp"

#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/cooldown_component.hpp"

namespace gw2combat::utils {

skill_castability_t can_cast_skill(const actor::skill_t& skill,
                                   entity_t actor_entity,
                                   registry_t& registry) {
    if (registry.any_of<component::owner_component>(actor_entity)) {
        return {.can_cast = true, .reason = "child actor skills are always castable"};
    }

    auto skill_entity = utils::get_skill_entity(skill, actor_entity, registry);
    // if (registry.any_of<component::internal_cooldown_component>(skill_entity)) {
    //     throw std::runtime_error(fmt::format("skill {} is on internal cooldown", skill));
    // }

    auto bundle_ptr = registry.try_get<component::bundle_component>(actor_entity);
    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0 && !(skill == "Weapon Swap" && bundle_ptr)) {
        std::string cooldown_component;
        for (auto&& [entity, cooldown, is_skill, owner] : registry
                                                              .view<component::cooldown_component,
                                                                    component::is_skill,
                                                                    component::owner_component>()
                                                              .each()) {
            if (owner.entity == actor_entity && is_skill.skill_configuration.skill_key == skill) {
                cooldown_component = fmt::format("{}", utils::to_string(cooldown));
                break;
            }
        }
        // spdlog::info("[{}] {}: can_cast_skill: skill {} doesn't have any more ammo, cooldown {}",
        //              utils::get_current_tick(registry),
        //              utils::get_entity_name(actor_entity, registry),
        //              to_string(skill),
        //              cooldown_component);
        return {.can_cast = false,
                .reason = skill_ammo.max_ammo > 1 ? "skill doesn't have any more ammo"
                                                  : "skill is on cooldown"};
    }

    auto& skill_configuration = utils::get_skill_configuration(skill, actor_entity, registry);
    if (skill_configuration.required_bundle.empty()) {
        if (skill_configuration.weapon_type != actor::weapon_type::INVALID &&
            skill_configuration.weapon_type != actor::weapon_type::EMPTY_HANDED &&
            skill_configuration.weapon_type != actor::weapon_type::MAIN_HAND &&
            skill_configuration.weapon_type != actor::weapon_type::KIT_CONJURE &&
            skill_configuration.weapon_type != actor::weapon_type::TOME) {
            auto& weapons = registry.get<component::equipped_weapons>(actor_entity).weapons;
            auto current_weapon_set = registry.get<component::current_weapon_set>(actor_entity).set;
            bool skill_available_on_weapon =
                std::any_of(weapons.begin(), weapons.end(), [&](const component::weapon_t& weapon) {
                    return weapon.set == current_weapon_set &&
                           weapon.type == skill_configuration.weapon_type;
                });
            if (!skill_available_on_weapon) {
                return {.can_cast = false, .reason = "skill not available on this weapon set"};
            }
        }
    } else {
        if (!bundle_ptr) {
            return {.can_cast = false,
                    .reason = fmt::format("skill requires bundle {}",
                                          skill_configuration.required_bundle)};
        } else if (skill_configuration.required_bundle != bundle_ptr->name) {
            return {.can_cast = false,
                    .reason = fmt::format("skill requires bundle {}, but currently have {}",
                                          skill_configuration.required_bundle,
                                          bundle_ptr->name)};
        }
    }

    auto condition_result = independent_conditions_satisfied(
        skill_configuration.cast_condition, actor_entity, std::nullopt, registry);
    if (!condition_result.satisfied) {
        return {.can_cast = false,
                .reason =
                    fmt::format("skill cast condition not satisfied: {}", condition_result.reason)};
    }
    return {.can_cast = true, .reason = ""};
}

entity_t get_skill_entity(const actor::skill_t& skill,
                          entity_t actor_entity,
                          registry_t& registry) {
    for (auto&& [skill_entity, owner_component, is_skill] :
         registry.view<component::owner_component, component::is_skill>().each()) {
        if (owner_component.entity == actor_entity &&
            is_skill.skill_configuration.skill_key == skill) {
            return skill_entity;
        }
    }
    throw std::runtime_error(fmt::format(
        "skill {} not found for actor {}", skill, utils::get_entity_name(actor_entity, registry)));
}

configuration::skill_t& get_skill_configuration(const actor::skill_t& skill,
                                                entity_t actor_entity,
                                                registry_t& registry) {
    for (auto&& [skill_entity, owner_component, is_skill] :
         registry.view<component::owner_component, component::is_skill>().each()) {
        if (owner_component.entity == actor_entity &&
            is_skill.skill_configuration.skill_key == skill) {
            return is_skill.skill_configuration;
        }
    }
    throw std::runtime_error(fmt::format(
        "skill {} not found for actor {}", skill, utils::get_entity_name(actor_entity, registry)));
}

bool skill_has_tag(const configuration::skill_t& skill, const actor::skill_tag_t& skill_tag) {
    return std::find(skill.tags.cbegin(), skill.tags.cend(), skill_tag) != skill.tags.cend();
}

void put_skill_on_cooldown(const actor::skill_t& skill,
                           entity_t actor_entity,
                           registry_t& registry,
                           bool force) {
    auto skill_entity = utils::get_skill_entity(skill, actor_entity, registry);

    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0 && !force) {
        utils::log_component<component::cooldown_component>(registry);
        throw std::runtime_error(
            fmt::format("put_skill_on_cooldown: actor {} skill {} doesn't have any more ammo",
                        get_entity_name(actor_entity, registry),
                        to_string(skill)));
    }
    skill_ammo.current_ammo = std::max(skill_ammo.current_ammo - 1, 0);

    auto& skill_configuration = utils::get_skill_configuration(skill, actor_entity, registry);
    if (!registry.any_of<component::cooldown_component>(skill_entity)) {
        registry.emplace_or_replace<component::cooldown_component>(
            skill_entity, component::cooldown_component{skill_configuration.cooldown});
    }
    spdlog::info("[{}] {}: put_skill_on_cooldown: skill {}",
                 utils::get_current_tick(registry),
                 get_entity_name(actor_entity, registry),
                 to_string(skill));
}

}  // namespace gw2combat::utils
