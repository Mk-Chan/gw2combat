#include "skill_utils.hpp"

#include "basic_utils.hpp"
#include "condition_utils.hpp"
#include "entity_utils.hpp"
#include "io_utils.hpp"

#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_conditional_skill_group.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/cooldown_component.hpp"

namespace gw2combat::utils {

skill_castability_t can_cast_skill(entity_t skill_entity, registry_t& registry) {
    auto& actor_entity = registry.get<component::owner_component>(skill_entity).entity;
    if (registry.any_of<component::owner_component>(actor_entity)) {
        return {.can_cast = true, .reason = "child actor skills are always castable"};
    }

    auto bundle_ptr = registry.try_get<component::bundle_component>(actor_entity);
    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    auto& skill_configuration = registry.get<component::is_skill>(skill_entity).skill_configuration;
    if (skill_ammo.current_ammo <= 0 &&
        !(skill_configuration.skill_key == "Weapon Swap" && bundle_ptr)) {
        // auto& cooldown_component = registry.get<component::cooldown_component>(skill_entity);
        // spdlog::info("[{}] {}: can_cast_skill: skill {} doesn't have any more ammo, cooldown {}",
        //              utils::get_current_tick(registry),
        //              utils::get_entity_name(actor_entity, registry),
        //              skill_configuration.skill_key,
        //              cooldown_component);
        return {.can_cast = false,
                .reason = skill_ammo.max_ammo > 1
                              ? "skill doesn't have any more ammo"
                              : "skill is on cooldown: " +
                                    utils::to_string(
                                        registry.get<component::cooldown_component>(skill_entity))};
    }

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
    std::string failure_reason = fmt::format(
        "skill {} not found for actor {}", skill, utils::get_entity_name(actor_entity, registry));

    for (auto&& [conditional_skill_group_entity, owner_component, is_conditional_skill_group] :
         registry.view<component::owner_component, component::is_conditional_skill_group>()
             .each()) {
        if (owner_component.entity == actor_entity &&
            is_conditional_skill_group.conditional_skill_group_configuration.skill_key == skill) {
            failure_reason =
                fmt::format("no condition satisfied in conditional skill group {} for actor {}",
                            skill,
                            utils::get_entity_name(actor_entity, registry));
            for (auto& conditional_skill_key :
                 is_conditional_skill_group.conditional_skill_group_configuration
                     .conditional_skill_keys) {
                auto skill_entity =
                    get_skill_entity(conditional_skill_key.skill_key, actor_entity, registry);
                if (utils::independent_conditions_satisfied(
                        conditional_skill_key.condition, actor_entity, std::nullopt, registry)
                        .satisfied) {
                    return skill_entity;
                }
            }
        }
    }
    throw std::runtime_error(failure_reason);
}

configuration::skill_t& get_skill_configuration(const actor::skill_t& skill,
                                                entity_t actor_entity,
                                                registry_t& registry) {
    auto skill_entity = utils::get_skill_entity(skill, actor_entity, registry);
    return registry.get<component::is_skill>(skill_entity).skill_configuration;
}

bool skill_has_tag(const configuration::skill_t& skill, const actor::skill_tag_t& skill_tag) {
    return std::find(skill.tags.cbegin(), skill.tags.cend(), skill_tag) != skill.tags.cend();
}

void put_skill_on_cooldown(entity_t skill_entity, registry_t& registry, bool force) {
    auto& skill_configuration = registry.get<component::is_skill>(skill_entity).skill_configuration;
    if (skill_configuration.cooldown[0] == 0) {
        return;
    }

    auto& skill_ammo = registry.get<component::ammo>(skill_entity);
    if (skill_ammo.current_ammo <= 0 && !force) {
        auto& cooldown_component = registry.get<component::cooldown_component>(skill_entity);
        throw std::runtime_error(fmt::format(
            "put_skill_on_cooldown: actor {} skill {} doesn't have any more ammo. cooldown: {}",
            utils::get_entity_name(registry.get<component::owner_component>(skill_entity).entity,
                                   registry),
            skill_configuration.skill_key,
            utils::to_string(cooldown_component)));
    }
    skill_ammo.current_ammo = std::max(skill_ammo.current_ammo - 1, 0);

    if (!registry.any_of<component::cooldown_component>(skill_entity)) {
        registry.emplace_or_replace<component::cooldown_component>(
            skill_entity, component::cooldown_component{skill_configuration.cooldown});
    }
    spdlog::info("[{}] {}: put_skill_on_cooldown: skill {}",
                 utils::get_current_tick(registry),
                 utils::get_entity_name(
                     registry.get<component::owner_component>(skill_entity).entity, registry),
                 skill_configuration.skill_key);
}

}  // namespace gw2combat::utils
