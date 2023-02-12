#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#include "common.hpp"

#include "entity_utils.hpp"

#include "actor/rotation.hpp"

#include "configuration/attribute_conversion.hpp"
#include "configuration/attribute_modifier.hpp"
#include "configuration/skill.hpp"
#include "configuration/unique_effect.hpp"

#include "component/actor/attribute_conversions_component.hpp"
#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skill_triggers_component.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/strike_counter.hpp"
#include "component/actor/team.hpp"
#include "component/actor/unique_effects_component.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/configuration_component.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal//duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

namespace gw2combat::utils {

static inline entity_t add_skill_to_actor(configuration::skill_t& skill,
                                          entity_t actor_entity,
                                          registry_t& registry) {
    auto skill_entity = registry.create();
    auto& skills_component = registry.get_or_emplace<component::skills_component>(actor_entity);
    skills_component.skill_entities.emplace_back(
        component::skill_entity{skill.skill_key, skill_entity});
    registry.emplace<component::is_skill>(skill_entity);
    registry.emplace<component::owner_component>(skill_entity, actor_entity);
    registry.emplace<component::ammo>(skill_entity, component::ammo{skill.ammo, skill.ammo});
    registry.emplace<component::configuration_component>(skill_entity, skill);
    if (!skill.attribute_modifiers.empty()) {
        auto& attribute_modifiers_component =
            registry.get_or_emplace<component::attribute_modifiers_component>(skill_entity);
        for (auto& attribute_modifier : skill.attribute_modifiers) {
            attribute_modifier.condition.depends_on_skill_off_cooldown = skill.skill_key;
            attribute_modifiers_component.attribute_modifiers.emplace_back(attribute_modifier);
        }
    }
    if (!skill.attribute_conversions.empty()) {
        auto& attribute_conversions_component =
            registry.get_or_emplace<component::attribute_conversions_component>(skill_entity);
        for (auto& attribute_conversion : skill.attribute_conversions) {
            attribute_conversion.condition.depends_on_skill_off_cooldown = skill.skill_key;
            attribute_conversions_component.attribute_conversions.emplace_back(
                attribute_conversion);
        }
    }
    registry.ctx().emplace_as<std::string>(skill_entity, skill.skill_key);
    return skill_entity;
}

static inline std::optional<entity_t> add_effect_to_actor(actor::effect_t effect,
                                                          int num_stacks,
                                                          int duration,
                                                          entity_t source_entity,
                                                          entity_t target_entity,
                                                          registry_t& registry) {
    for (; num_stacks > 0; --num_stacks) {
        auto effect_entity = registry.create();
        registry.emplace<component::duration_component>(effect_entity,
                                                        component::duration_component{duration, 0});
        registry.emplace<component::is_effect>(effect_entity, effect);
        registry.emplace<component::source_actor>(effect_entity, source_entity);
        registry.emplace<component::owner_component>(effect_entity, target_entity);
        registry.get_or_emplace<component::effects_component>(target_entity)
            .effect_entities.emplace_back(component::effect_entity{effect, effect_entity});

        if (effect == actor::effect_t::MIGHT) {
            auto& attribute_modifiers_component =
                registry.get_or_emplace<component::attribute_modifiers_component>(effect_entity);
            attribute_modifiers_component.attribute_modifiers.emplace_back(
                configuration::attribute_modifier_t{
                    configuration::condition_t{}, actor::attribute_t::POWER, 1.0, 30.0});
            attribute_modifiers_component.attribute_modifiers.emplace_back(
                configuration::attribute_modifier_t{
                    configuration::condition_t{}, actor::attribute_t::CONDITION_DAMAGE, 1.0, 30.0});
        } else if (effect == actor::effect_t::FURY) {
            auto& attribute_modifiers_component =
                registry.get_or_emplace<component::attribute_modifiers_component>(effect_entity);
            attribute_modifiers_component.attribute_modifiers.emplace_back(
                configuration::attribute_modifier_t{configuration::condition_t{},
                                                    actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER,
                                                    1.0,
                                                    25.0});
        } else if (effect == actor::effect_t::QUICKNESS) {
            registry.emplace_or_replace<component::has_quickness>(target_entity);
        } else if (effect == actor::effect_t::ALACRITY) {
            registry.emplace_or_replace<component::has_alacrity>(target_entity);
        }
    }
}

static inline std::optional<entity_t> add_unique_effect_to_actor(
    configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    registry_t& registry) {
    if (registry.any_of<component::unique_effects_component>(actor_entity) &&
        registry.get<component::unique_effects_component>(actor_entity)
                .count(unique_effect.unique_effect_key) >= unique_effect.max_stored_stacks) {
        return std::nullopt;
    }

    auto unique_effect_entity = registry.create();
    registry.emplace<component::is_unique_effect>(unique_effect_entity);
    registry.emplace<component::owner_component>(unique_effect_entity,
                                                 component::owner_component{actor_entity});

    auto& actor_unique_effects_component =
        registry.get_or_emplace<component::unique_effects_component>(actor_entity);
    actor_unique_effects_component.unique_effect_entities.emplace_back(
        component::unique_effect_entity{unique_effect.unique_effect_key, unique_effect_entity});

    if (!unique_effect.attribute_modifiers.empty()) {
        auto& attribute_modifiers_component =
            registry.get_or_emplace<component::attribute_modifiers_component>(unique_effect_entity);
        for (auto& attribute_modifier : unique_effect.attribute_modifiers) {
            attribute_modifiers_component.attribute_modifiers.emplace_back(attribute_modifier);
        }
    }
    if (!unique_effect.attribute_conversions.empty()) {
        auto& attribute_conversions_component =
            registry.get_or_emplace<component::attribute_conversions_component>(
                unique_effect_entity);
        for (auto& attribute_conversion : unique_effect.attribute_conversions) {
            attribute_conversions_component.attribute_conversions.emplace_back(
                attribute_conversion);
        }
    }
    if (!unique_effect.skill_triggers.empty()) {
        auto& skill_triggers_component =
            registry.get_or_emplace<component::skill_triggers_component>(unique_effect_entity);
        for (auto& skill_trigger : unique_effect.skill_triggers) {
            skill_triggers_component.skill_triggers.emplace_back(skill_trigger);
            // Add skill to actor? Be careful because then the actor will be allowed to cast this
            // skill in a non-triggered as well as receive any passive bonuses as well.
            // Maybe make a temporary entity for triggered skills, so they can track cds separately?
            // For example radiant fire.
        }
    }

    registry.ctx().emplace_as<std::string>(unique_effect_entity, utils::to_string(unique_effect));

    return unique_effect_entity;
}

[[nodiscard]] static inline entity_t create_child_actor(entity_t parent_actor,
                                                        const std::string& name,
                                                        int team_id,
                                                        registry_t& registry) {
    auto child_actor = registry.create();
    registry.ctx().emplace_as<std::string>(child_actor,
                                           fmt::format("child_actor{}-{}", child_actor, name));
    registry.emplace<component::is_actor>(child_actor);
    registry.emplace<component::owner_component>(
        child_actor, component::owner_component{utils::get_owner(parent_actor, registry)});
    registry.emplace<component::destroy_after_rotation>(child_actor);
    registry.emplace<component::team>(child_actor, component::team{team_id});
    return child_actor;
}

static inline void enqueue_child_skills(entity_t parent_actor,
                                        const std::string& child_name,
                                        std::vector<configuration::skill_t>& skills,
                                        registry_t& registry) {
    if (!skills.empty()) {
        auto child_actor = utils::create_child_actor(
            parent_actor, child_name, registry.get<component::team>(parent_actor).id, registry);
        // spdlog::info("[{}]: spawned {}",
        //              utils::get_current_tick(registry),
        //              utils::get_entity_name(child_actor, registry));
        actor::rotation_t rotation;
        for (auto& skill : skills) {
            utils::add_skill_to_actor(skill, child_actor, registry);
            rotation.skill_casts.emplace_back(actor::skill_cast_t{skill.skill_key, 0});
        }
        registry.emplace<component::rotation_component>(
            child_actor, component::rotation_component{rotation, 0, false});
    }
}

static inline void enqueue_child_skill(entity_t parent_actor,
                                       configuration::skill_t& skill,
                                       registry_t& registry) {
    auto child_actor = utils::create_child_actor(parent_actor,
                                                 skill.skill_key + " Entity",
                                                 registry.get<component::team>(parent_actor).id,
                                                 registry);
    actor::rotation_t rotation;
    utils::add_skill_to_actor(skill, child_actor, registry);
    rotation.skill_casts.emplace_back(actor::skill_cast_t{skill.skill_key, 0});
    registry.emplace<component::rotation_component>(
        child_actor, component::rotation_component{rotation, 0, false});
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
