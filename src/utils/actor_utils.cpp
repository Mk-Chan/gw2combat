#include "actor_utils.hpp"

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
#include "component/actor/finished_casting_skills.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/rotation_component.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/actor/team.hpp"
#include "component/attributes/is_attribute_conversion.hpp"
#include "component/attributes/is_attribute_modifier.hpp"
#include "component/counter/is_counter_modifier.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_effect_removal.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/lifecycle/destroy_entity.hpp"
#include "component/skill/ammo.hpp"
#include "component/skill/is_skill.hpp"
#include "component/temporal/duration_component.hpp"
#include "component/temporal/has_alacrity.hpp"
#include "component/temporal/has_quickness.hpp"

namespace gw2combat::utils {

entity_t create_temporary_rotation_child_actor(entity_t parent_actor,
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

entity_t add_skill_to_actor(const configuration::skill_t& skill,
                            entity_t actor_entity,
                            registry_t& registry) {
    for (auto&& [skill_entity, owner_component, is_skill] :
         registry.view<component::owner_component, component::is_skill>().each()) {
        if (owner_component.entity == actor_entity && is_skill.skill_configuration == skill) {
            return skill_entity;
        }
    }

    auto skill_entity = registry.create();
    registry.ctx().emplace_as<std::string>(skill_entity, skill.skill_key + " skill holder entity");

    registry.emplace<component::is_skill>(skill_entity, skill);
    registry.emplace<component::owner_component>(skill_entity, actor_entity);

    registry.emplace<component::ammo>(skill_entity, component::ammo{skill.ammo, skill.ammo});

    utils::add_owner_based_component<std::vector<configuration::attribute_conversion_t>,
                                     component::is_attribute_conversion>(
        skill.attribute_conversions, actor_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                     component::is_attribute_modifier>(
        skill.attribute_modifiers, actor_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::counter_modifier_t>,
                                     component::is_counter_modifier_t>(
        skill.counter_modifiers, actor_entity, registry);

    for (auto& child_skill : skill.child_skill_keys) {
        auto& child_skill_configuration = utils::get_skill_configuration(
            child_skill, utils::get_owner(actor_entity, registry), registry);
        add_skill_to_actor(child_skill_configuration, actor_entity, registry);
    }

    return skill_entity;
}

void enqueue_child_skills(entity_t parent_actor,
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

void enqueue_child_skill(const actor::skill_t& skill, entity_t parent_actor, registry_t& registry) {
    std::vector<actor::skill_t> skills;
    skills.emplace_back(skill);
    enqueue_child_skills(parent_actor, "Temporary " + skill + " Entity", skills, registry);
}

entity_t add_effect_to_actor(actor::effect_t effect,
                             entity_t actor_entity,
                             entity_t source_actor,
                             const actor::skill_t& source_skill,
                             int duration,
                             int grouped_with_num_stacks,
                             registry_t& registry) {
    // TODO: Check for crossing maximum stacks here.
    auto stacking_type = utils::get_effect_stacking_type(effect);
    if (stacking_type == actor::stacking_t::STACKING_DURATION ||
        stacking_type == actor::stacking_t::REPLACE) {
        auto is_effect_view = registry.view<component::is_effect, component::owner_component>();
        for (auto&& [effect_entity, is_effect, owner_component] : is_effect_view.each()) {
            auto owner_actor = utils::get_owner(owner_component.entity, registry);
            if (owner_actor != actor_entity || is_effect.effect != effect) {
                continue;
            }

            auto& duration_component = registry.get<component::duration_component>(effect_entity);
            if (stacking_type == actor::stacking_t::STACKING_DURATION) {
                duration_component.duration = std::min(duration_component.duration + duration,
                                                       utils::get_max_effect_duration(effect));
                int remaining_duration = duration_component.duration - duration_component.progress;
                duration_component.progress = 0;
                duration_component.duration =
                    std::min(remaining_duration + duration, utils::get_max_effect_duration(effect));
                return effect_entity;
            } else {
                duration_component.progress = duration_component.duration;
                registry.emplace_or_replace<component::destroy_entity>(effect_entity);
                break;
            }
        }
    }
    auto effect_entity = registry.create();
    registry.ctx().emplace_as<std::string>(effect_entity,
                                           utils::to_string(effect) + " effect holder entity");

    registry.emplace<component::is_effect>(effect_entity,
                                           component::is_effect{effect, grouped_with_num_stacks});
    if (utils::is_damaging_condition(effect)) {
        registry.emplace<component::is_damaging_effect>(effect_entity);
    }
    registry.emplace<component::owner_component>(effect_entity, actor_entity);
    registry.emplace<component::source_actor>(effect_entity, source_actor);
    registry.emplace<component::source_skill>(effect_entity, source_skill);
    registry.emplace<component::duration_component>(effect_entity,
                                                    component::duration_component{duration, 0});

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

    return effect_entity;
}

std::optional<entity_t> add_permanent_effect_to_actor(actor::effect_t effect,
                                                      entity_t actor_entity,
                                                      registry_t& registry) {
    return add_effect_to_actor(
        effect, actor_entity, utils::get_singleton_entity(), {}, 1'000'000'000, 1, registry);
}

std::vector<entity_t> add_effect_to_actor(actor::effect_t effect,
                                          int num_stacks,
                                          int duration,
                                          const actor::skill_t& source_skill,
                                          entity_t source_entity,
                                          entity_t target_entity,
                                          registry_t& registry) {
    std::vector<entity_t> effect_entities;
    for (int i = 0; i < num_stacks; ++i) {
        auto effect_entity = add_effect_to_actor(
            effect, target_entity, source_entity, source_skill, duration, num_stacks, registry);
        effect_entities.emplace_back(effect_entity);
    }
    return effect_entities;
}

std::optional<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    entity_t source_actor,
    const actor::skill_t& source_skill,
    int duration,
    registry_t& registry) {
    int stacks_count = 0;
    auto unique_effect_owners =
        registry.view<component::is_unique_effect, component::owner_component>().each();
    for (auto&& [unique_effect_entity, is_unique_effect, owner_component] : unique_effect_owners) {
        if (owner_component.entity != actor_entity ||
            is_unique_effect.unique_effect.unique_effect_key != unique_effect.unique_effect_key) {
            continue;
        }
        ++stacks_count;
        auto& duration_component =
            registry.get<component::duration_component>(unique_effect_entity);
        if (unique_effect.stacking_type == actor::stacking_t::STACKING_DURATION) {
            int remaining_duration = duration_component.duration - duration_component.progress;
            duration_component.progress = 0;
            duration_component.duration = std::min(remaining_duration + duration,
                                                   is_unique_effect.unique_effect.max_duration);
            return unique_effect_entity;
        } else if (unique_effect.stacking_type == actor::stacking_t::REPLACE) {
            duration_component.progress = duration_component.duration;
            registry.emplace_or_replace<component::destroy_entity>(unique_effect_entity);
        }
    }
    if (stacks_count >= unique_effect.max_stored_stacks) {
        return std::nullopt;
    }

    auto unique_effect_entity = registry.create();
    registry.ctx().emplace_as<std::string>(
        unique_effect_entity, unique_effect.unique_effect_key + " unique-effect holder entity");

    registry.emplace<component::is_unique_effect>(unique_effect_entity, unique_effect);
    registry.emplace<component::owner_component>(unique_effect_entity, actor_entity);
    registry.emplace<component::source_actor>(unique_effect_entity, source_actor);
    registry.emplace<component::source_skill>(unique_effect_entity, source_skill);
    registry.emplace<component::duration_component>(unique_effect_entity,
                                                    component::duration_component{duration, 0});

    utils::add_owner_based_component<std::vector<configuration::attribute_conversion_t>,
                                     component::is_attribute_conversion>(
        unique_effect.attribute_conversions, unique_effect_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::attribute_modifier_t>,
                                     component::is_attribute_modifier>(
        unique_effect.attribute_modifiers, unique_effect_entity, registry);
    utils::add_owner_based_component<std::vector<configuration::counter_modifier_t>,
                                     component::is_counter_modifier_t>(
        unique_effect.counter_modifiers, unique_effect_entity, registry);
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

    return unique_effect_entity;
}

std::optional<entity_t> add_permanent_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    entity_t actor_entity,
    registry_t& registry) {
    return add_unique_effect_to_actor(
        unique_effect, actor_entity, utils::get_singleton_entity(), {}, 1'000'000'000, registry);
}

std::vector<entity_t> add_unique_effect_to_actor(
    const configuration::unique_effect_t& unique_effect,
    int num_stacks,
    int duration,
    const actor::skill_t& source_skill,
    entity_t source_entity,
    entity_t target_entity,
    registry_t& registry) {
    std::vector<entity_t> unique_effect_entities;
    for (; num_stacks > 0; --num_stacks) {
        add_unique_effect_to_actor(
            unique_effect, target_entity, source_entity, source_skill, duration, registry);
    }
    return unique_effect_entities;
}

void finish_casting_skill(entity_t actor_entity, entity_t skill_entity, registry_t& registry) {
    auto& finished_casting_skills =
        registry.get_or_emplace<component::finished_casting_skills>(actor_entity);
    finished_casting_skills.skill_entities.emplace_back(skill_entity);

    auto& skill_configuration = registry.get<component::is_skill>(skill_entity).skill_configuration;
    if (skill_configuration.cooldown[0] != 0 &&
        !(skill_configuration.skill_key == "Weapon Swap" &&
          registry.any_of<component::bundle_component>(actor_entity))) {
        utils::put_skill_on_cooldown(skill_configuration.skill_key, actor_entity, registry);
        for (auto& skill_to_put_on_cooldown : skill_configuration.skills_to_put_on_cooldown) {
            utils::put_skill_on_cooldown(skill_to_put_on_cooldown, actor_entity, registry, true);
        }
        auto owner_entity = utils::get_owner(actor_entity, registry);
        if (actor_entity != owner_entity) {
            utils::put_skill_on_cooldown(skill_configuration.skill_key, owner_entity, registry);
            for (auto& skill_to_put_on_cooldown : skill_configuration.skills_to_put_on_cooldown) {
                utils::put_skill_on_cooldown(
                    skill_to_put_on_cooldown, actor_entity, registry, true);
            }
        }
    }
    spdlog::info("[{}] {}: finished casting skill {}",
                 utils::get_current_tick(registry),
                 utils::get_entity_name(actor_entity, registry),
                 utils::to_string(skill_configuration.skill_key));
}

}  // namespace gw2combat::utils
