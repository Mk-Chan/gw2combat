#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#include "common.hpp"

#include "effect_utils.hpp"
#include "entity_utils.hpp"
#include "skill_utils.hpp"

#include "actor/rotation.hpp"

#include "configuration/attribute_conversion.hpp"
#include "configuration/attribute_modifier.hpp"
#include "configuration/skill.hpp"
#include "configuration/unique_effect.hpp"

#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/attributes/is_attribute_conversion.hpp"
#include "component/attributes/is_attribute_modifier.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_effect_removal.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

namespace gw2combat::utils {

template <typename ConfigurationType, typename ComponentType>
static inline entity_t add_owner_based_component(const ConfigurationType& configuration_type_value,
                                                 entity_t parent_entity,
                                                 registry_t& registry) {
    auto component_type_holder_entity = registry.create();
    registry.emplace<component::owner_component>(component_type_holder_entity, parent_entity);
    registry.emplace<ComponentType>(component_type_holder_entity, configuration_type_value);
    return component_type_holder_entity;
}

[[nodiscard]] static inline entity_t create_temporary_rotation_child_actor(entity_t parent_actor,
                                                                           const std::string& name,
                                                                           int team_id,
                                                                           registry_t& registry) {
    auto child_actor_entity = registry.create();
    registry.ctx().emplace_as<std::string>(
        child_actor_entity, fmt::format("child_actor{}-{}", child_actor_entity, name));
    registry.emplace<component::is_actor>(child_actor_entity);
    registry.emplace<component::owner_component>(child_actor_entity,
                                                 utils::get_owner(parent_actor, registry));
    registry.emplace<component::team>(child_actor_entity, team_id);
    registry.emplace<component::destroy_after_rotation>(child_actor_entity);
    registry.emplace_or_replace<component::actor_created>(child_actor_entity);
    return child_actor_entity;
}

static inline entity_t add_skill_to_actor(const configuration::skill_t& skill,
                                          entity_t actor_entity,
                                          registry_t& registry) {
    for (auto&& [skill_entity, owner_component, is_skill] :
         registry.view<component::owner_component, component::is_skill>().each()) {
        if (owner_component.entity == actor_entity && is_skill.skill_configuration == skill) {
            return skill_entity;
        }
    }

    auto skill_entity = registry.create();
    registry.emplace<component::is_skill>(skill_entity, skill);
    registry.emplace<component::owner_component>(skill_entity, actor_entity);

    registry.emplace<component::ammo>(skill_entity, component::ammo{skill.ammo, skill.ammo});

    utils::add_owner_based_component<std::vector<configuration::attribute_conversion_t>,
                                     component::is_attribute_conversion>(
        skill.attribute_conversions, actor_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                     component::is_attribute_modifier>(
        skill.attribute_modifiers, actor_entity, registry);

    registry.ctx().emplace_as<std::string>(skill_entity, skill.skill_key);

    for (auto& child_skill : skill.child_skill_keys) {
        auto& child_skill_configuration = utils::get_skill_configuration(
            child_skill, utils::get_owner(actor_entity, registry), registry);
        add_skill_to_actor(child_skill_configuration, actor_entity, registry);
    }

    return skill_entity;
}

static inline void enqueue_child_skills(entity_t parent_actor,
                                        const std::string& child_name,
                                        const std::vector<actor::skill_t>& skills,
                                        registry_t& registry) {
    if (skills.empty()) {
        return;
    }

    auto child_actor = utils::create_temporary_rotation_child_actor(
        parent_actor, child_name, registry.get<component::team>(parent_actor).id, registry);

    actor::rotation_t rotation;
    for (auto& skill : skills) {
        auto& skill_configuration = utils::get_skill_configuration(skill, parent_actor, registry);
        utils::add_skill_to_actor(skill_configuration, child_actor, registry);
        rotation.skill_casts.emplace_back(actor::skill_cast_t{skill_configuration.skill_key, 0});
    }
    registry.emplace<component::rotation_component>(
        child_actor, component::rotation_component{rotation, 0, false});

    spdlog::info("[{}] {}: spawned {}",
                 utils::get_current_tick(registry),
                 utils::get_entity_name(parent_actor, registry),
                 utils::get_entity_name(child_actor, registry));
}

static inline void enqueue_child_skill(const actor::skill_t& skill,
                                       entity_t parent_actor,
                                       registry_t& registry) {
    std::vector<actor::skill_t> skills;
    skills.emplace_back(skill);
    enqueue_child_skills(parent_actor, "Temporary " + skill + " Entity", skills, registry);
}

static inline std::optional<entity_t> add_effect_to_actor(actor::effect_t effect,
                                                          entity_t actor_entity,
                                                          registry_t& registry) {
    // TODO: Check for crossing maximum stacks / duration here.
    // TODO: Differentiate between stacking duration / intensity

    auto effect_entity = registry.create();
    registry.emplace<component::is_effect>(effect_entity, effect);
    if (utils::is_damaging_condition(effect)) {
        registry.emplace<component::is_damaging_effect>(effect_entity);
    }
    registry.emplace<component::owner_component>(effect_entity, actor_entity);
    registry.emplace<component::source_actor>(effect_entity, utils::get_singleton_entity());
    registry.emplace<component::source_skill>(effect_entity, component::source_skill{});
    registry.emplace<component::duration_component>(
        effect_entity, component::duration_component{1'000'000'000, 0});

    if (effect == actor::effect_t::MIGHT) {
        utils::add_owner_based_component<std::vector<configuration::attribute_conversion_t>,
                                         component::is_attribute_conversion>(
            {configuration::attribute_conversion_t{configuration::condition_t{},
                                                   actor::attribute_t::CONDITION_DAMAGE,
                                                   actor::attribute_t::CONDITION_DAMAGE,
                                                   0.0,
                                                   30.0},
             configuration::attribute_conversion_t{configuration::condition_t{},
                                                   actor::attribute_t::POWER,
                                                   actor::attribute_t::POWER,
                                                   0.0,
                                                   30.0}},
            effect_entity,
            registry);
    } else if (effect == actor::effect_t::FURY) {
        utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                         component::is_attribute_modifier>(
            {configuration::attribute_modifier_t{configuration::condition_t{},
                                                 actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER,
                                                 1.0,
                                                 0.25}},
            effect_entity,
            registry);
    } else if (effect == actor::effect_t::QUICKNESS) {
        registry.emplace_or_replace<component::has_quickness>(actor_entity);
    } else if (effect == actor::effect_t::ALACRITY) {
        registry.emplace_or_replace<component::has_alacrity>(actor_entity);
    } else if (effect == actor::effect_t::VULNERABILITY) {
        utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                         component::is_attribute_modifier>(
            {configuration::attribute_modifier_t{
                 configuration::condition_t{},
                 actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
                 1.0,
                 0.01},
             configuration::attribute_modifier_t{
                 configuration::condition_t{},
                 actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
                 1.0,
                 0.01}},
            effect_entity,
            registry);
    }

    registry.ctx().emplace_as<std::string>(effect_entity, utils::to_string(effect));

    return effect_entity;
}

static inline std::vector<entity_t> add_effect_to_actor(actor::effect_t effect,
                                                        int num_stacks,
                                                        int duration,
                                                        const actor::skill_t& source_skill,
                                                        entity_t source_entity,
                                                        entity_t target_entity,
                                                        registry_t& registry) {
    std::vector<entity_t> effect_entities;
    for (; num_stacks > 0; --num_stacks) {
        std::optional<entity_t> effect_entity =
            add_effect_to_actor(effect, target_entity, registry);
        if (effect_entity) {
            effect_entities.emplace_back(*effect_entity);
            registry.emplace_or_replace<component::source_actor>(
                *effect_entity, component::source_actor{source_entity});
            registry.emplace_or_replace<component::source_skill>(
                *effect_entity, component::source_skill{source_skill});
            registry.emplace_or_replace<component::duration_component>(
                *effect_entity, component::duration_component{duration, 0});
        }
    }
    return effect_entities;
}

static inline std::optional<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    registry_t& registry) {
    int stacks_count = 0;
    auto unique_effect_owners =
        registry.view<component::is_unique_effect, component::owner_component>().each();
    for (auto&& [unique_effect_entity, is_unique_effect, owner_component] : unique_effect_owners) {
        if (is_unique_effect.unique_effect.unique_effect_key == unique_effect.unique_effect_key &&
            owner_component.entity == actor_entity) {
            ++stacks_count;
        }
    }
    if (stacks_count >= unique_effect.max_stored_stacks) {
        return std::nullopt;
    }

    auto unique_effect_entity = registry.create();
    registry.emplace<component::is_unique_effect>(unique_effect_entity, unique_effect);
    registry.emplace<component::owner_component>(unique_effect_entity, actor_entity);
    registry.emplace<component::source_actor>(unique_effect_entity, actor_entity);
    registry.emplace<component::source_skill>(unique_effect_entity, component::source_skill{});
    registry.emplace<component::duration_component>(
        unique_effect_entity, component::duration_component{1'000'000'000, 0});

    utils::add_owner_based_component<std::vector<configuration::attribute_conversion_t>,
                                     component::is_attribute_conversion>(
        unique_effect.attribute_conversions, unique_effect_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                     component::is_attribute_modifier>(
        unique_effect.attribute_modifiers, unique_effect_entity, registry);
    for (auto& skill_trigger : unique_effect.skill_triggers) {
        utils::add_owner_based_component<configuration::skill_trigger_t,
                                         component::is_skill_trigger>(
            skill_trigger, unique_effect_entity, registry);
    }
    for (auto& skill_trigger : unique_effect.unchained_skill_triggers) {
        utils::add_owner_based_component<configuration::skill_trigger_t,
                                         component::is_unchained_skill_trigger>(
            skill_trigger, unique_effect_entity, registry);
    }
    for (auto& effect_removal : unique_effect.effect_removals) {
        utils::add_owner_based_component<configuration::effect_removal_t,
                                         component::is_effect_removal>(
            effect_removal, unique_effect_entity, registry);
    }

    registry.ctx().emplace_as<std::string>(unique_effect_entity, unique_effect.unique_effect_key);

    return unique_effect_entity;
}

static inline std::vector<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    int num_stacks,
    int duration,
    const actor::skill_t& source_skill,
    entity_t source_entity,
    entity_t target_entity,
    registry_t& registry) {
    std::vector<entity_t> unique_effect_entities;
    for (; num_stacks > 0; --num_stacks) {
        std::optional<entity_t> unique_effect_entity =
            add_unique_effect_to_actor(unique_effect, target_entity, registry);
        if (unique_effect_entity) {
            unique_effect_entities.emplace_back(*unique_effect_entity);
            registry.emplace_or_replace<component::source_actor>(
                *unique_effect_entity, component::source_actor{source_entity});
            registry.emplace_or_replace<component::source_skill>(
                *unique_effect_entity, component::source_skill{source_skill});
            registry.emplace_or_replace<component::duration_component>(
                *unique_effect_entity, component::duration_component{duration, 0});
        }
    }
    return unique_effect_entities;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
