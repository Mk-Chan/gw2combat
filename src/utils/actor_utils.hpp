#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILS_HPP

#include "common.hpp"

#include "effect_utils.hpp"
#include "entity_utils.hpp"

#include "actor/rotation.hpp"

#include "configuration/attribute_conversion.hpp"
#include "configuration/attribute_modifier.hpp"
#include "configuration/skill.hpp"
#include "configuration/unique_effect.hpp"

#include "component/actor/attribute_conversions_component.hpp"
#include "component/actor/attribute_modifiers_component.hpp"
#include "component/actor/destroy_after_rotation.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/no_more_rotation.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/skill_triggers_component.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/actor/unchained_skill_triggers_component.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_effect_removal.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/skill/skill_configuration_component.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline entity_t create_persistent_child_actor(entity_t parent_actor,
                                                                   const std::string& name,
                                                                   int team_id,
                                                                   registry_t& registry) {
    auto child_actor = registry.create();
    registry.ctx().emplace_as<std::string>(child_actor,
                                           fmt::format("child_actor{}-{}", child_actor, name));
    registry.emplace<component::is_actor>(child_actor);
    registry.emplace<component::owner_component>(
        child_actor, component::owner_component{utils::get_owner(parent_actor, registry)});
    registry.emplace<component::team>(child_actor, component::team{team_id});
    return child_actor;
}

[[nodiscard]] static inline entity_t create_temporary_rotation_child_actor(entity_t parent_actor,
                                                                           const std::string& name,
                                                                           int team_id,
                                                                           registry_t& registry) {
    auto child_actor = create_persistent_child_actor(parent_actor, name, team_id, registry);
    registry.emplace<component::destroy_after_rotation>(child_actor);
    return child_actor;
}

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
    registry.emplace<component::skill_configuration_component>(skill_entity, skill);

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

static inline void enqueue_child_skills(entity_t parent_actor,
                                        const std::string& child_name,
                                        std::vector<configuration::skill_t>& skills,
                                        registry_t& registry) {
    if (skills.empty()) {
        return;
    }
    auto child_actor = utils::create_temporary_rotation_child_actor(
        parent_actor, child_name, registry.get<component::team>(parent_actor).id, registry);
    actor::rotation_t rotation;
    for (auto& skill : skills) {
        utils::add_skill_to_actor(skill, child_actor, registry);
        rotation.skill_casts.emplace_back(actor::skill_cast_t{skill.skill_key, 0});
    }
    registry.emplace<component::rotation_component>(
        child_actor, component::rotation_component{rotation, 0, false});
    spdlog::info("[{}] {}: spawned {}",
                 utils::get_current_tick(registry),
                 utils::get_entity_name(parent_actor, registry),
                 utils::get_entity_name(child_actor, registry));
}

static inline void enqueue_child_skill(entity_t parent_actor,
                                       configuration::skill_t& skill,
                                       registry_t& registry) {
    auto child_actor =
        utils::create_temporary_rotation_child_actor(parent_actor,
                                                     skill.skill_key + " Entity",
                                                     registry.get<component::team>(parent_actor).id,
                                                     registry);
    actor::rotation_t rotation;
    utils::add_skill_to_actor(skill, child_actor, registry);
    rotation.skill_casts.emplace_back(actor::skill_cast_t{skill.skill_key, 0});
    registry.emplace<component::rotation_component>(
        child_actor, component::rotation_component{rotation, 0, false});
}

static inline void enqueue_triggered_skill(entity_t parent_actor,
                                           const actor::skill_t& skill,
                                           registry_t& registry) {
    registry.view<component::is_skill_trigger_tracker>().each(
        [&](entity_t skill_trigger_tracker_entity,
            const component::is_skill_trigger_tracker& is_skill_trigger_tracker) {
            if (utils::get_owner(skill_trigger_tracker_entity, registry) != parent_actor ||
                is_skill_trigger_tracker.skill != skill) {
                return;
            }
            actor::rotation_t rotation;
            rotation.skill_casts.emplace_back(actor::skill_cast_t{skill, 0});
            registry.remove<component::no_more_rotation>(skill_trigger_tracker_entity);
            registry.emplace_or_replace<component::rotation_component>(
                skill_trigger_tracker_entity, component::rotation_component{rotation, 0, false});
        });
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
    registry.emplace<component::source_actor>(effect_entity, utils::get_console_entity());
    registry.emplace<component::source_skill>(effect_entity, component::source_skill{});
    registry.emplace<component::duration_component>(
        effect_entity, component::duration_component{1'000'000'000, 0});

    if (effect == actor::effect_t::MIGHT) {
        auto& attribute_conversions_component =
            registry.get_or_emplace<component::attribute_conversions_component>(effect_entity);
        attribute_conversions_component.attribute_conversions.emplace_back(
            configuration::attribute_conversion_t{configuration::condition_t{},
                                                  actor::attribute_t::POWER,
                                                  actor::attribute_t::POWER,
                                                  0.0,
                                                  30.0});
        attribute_conversions_component.attribute_conversions.emplace_back(
            configuration::attribute_conversion_t{configuration::condition_t{},
                                                  actor::attribute_t::CONDITION_DAMAGE,
                                                  actor::attribute_t::CONDITION_DAMAGE,
                                                  0.0,
                                                  30.0});
    } else if (effect == actor::effect_t::FURY) {
        auto& attribute_modifiers_component =
            registry.get_or_emplace<component::attribute_modifiers_component>(effect_entity);
        attribute_modifiers_component.attribute_modifiers.emplace_back(
            configuration::attribute_modifier_t{configuration::condition_t{},
                                                actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER,
                                                1.0,
                                                0.25});
    } else if (effect == actor::effect_t::QUICKNESS) {
        registry.emplace_or_replace<component::has_quickness>(actor_entity);
    } else if (effect == actor::effect_t::ALACRITY) {
        registry.emplace_or_replace<component::has_alacrity>(actor_entity);
    } else if (effect == actor::effect_t::VULNERABILITY) {
        auto& attribute_modifiers_component =
            registry.get_or_emplace<component::attribute_modifiers_component>(effect_entity);
        attribute_modifiers_component.attribute_modifiers.emplace_back(
            configuration::attribute_modifier_t{
                configuration::condition_t{},
                actor::attribute_t::INCOMING_STRIKE_DAMAGE_MULTIPLIER_ADD_GROUP,
                1.0,
                0.01});
        attribute_modifiers_component.attribute_modifiers.emplace_back(
            configuration::attribute_modifier_t{
                configuration::condition_t{},
                actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP,
                1.0,
                0.01});
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
    registry.emplace<component::is_unique_effect>(unique_effect_entity,
                                                  component::is_unique_effect{unique_effect});
    registry.emplace<component::owner_component>(unique_effect_entity,
                                                 component::owner_component{actor_entity});
    registry.emplace<component::source_actor>(unique_effect_entity, actor_entity);
    registry.emplace<component::source_skill>(unique_effect_entity, component::source_skill{});
    registry.emplace<component::duration_component>(
        unique_effect_entity, component::duration_component{1'000'000'000, 0});

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
        auto& actor_skills_component = registry.get<component::skills_component>(actor_entity);
        auto& skill_triggers_component =
            registry.get_or_emplace<component::skill_triggers_component>(unique_effect_entity);
        for (auto& skill_trigger : unique_effect.skill_triggers) {
            auto parent_skill_entity = actor_skills_component.find_by(skill_trigger.skill_key);
            auto& skill_configuration =
                registry.get<component::skill_configuration_component>(parent_skill_entity)
                    .skill_configuration;

            auto skill_trigger_tracker_entity = create_persistent_child_actor(
                actor_entity,
                "Triggered " + skill_trigger.skill_key + " Tracker Entity",
                registry.get<component::team>(actor_entity).id,
                registry);
            auto tracker_skill_entity = utils::add_skill_to_actor(
                skill_configuration, skill_trigger_tracker_entity, registry);
            registry.emplace<component::is_skill_trigger_tracker>(
                skill_trigger_tracker_entity,
                component::is_skill_trigger_tracker{skill_trigger.skill_key, tracker_skill_entity});

            skill_triggers_component.skill_triggers.emplace_back(skill_trigger);
        }
    }
    if (!unique_effect.unchained_skill_triggers.empty()) {
        auto& unchained_skill_triggers_component =
            registry.get_or_emplace<component::unchained_skill_triggers_component>(
                unique_effect_entity);
        std::copy(unique_effect.unchained_skill_triggers.begin(),
                  unique_effect.unchained_skill_triggers.end(),
                  std::back_inserter(unchained_skill_triggers_component.skill_triggers));
    }

    for (auto& effect_removal : unique_effect.effect_removals) {
        auto effect_removal_entity = registry.create();
        registry.emplace<component::is_effect_removal>(effect_removal_entity, effect_removal);
        registry.emplace<component::owner_component>(effect_removal_entity, actor_entity);
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
