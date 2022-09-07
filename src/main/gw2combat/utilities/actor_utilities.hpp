#ifndef GW2COMBAT_UTILITIES_ACTOR_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_ACTOR_UTILITIES_HPP

#include "gw2combat/common.hpp"

#include "entity_utilities.hpp"

#include "gw2combat/actor/build.hpp"
#include "gw2combat/actor/rotation.hpp"
#include "gw2combat/actor/skill_database.hpp"
#include "gw2combat/actor/trait_database.hpp"
#include "gw2combat/actor/unique_effect_database.hpp"

#include "gw2combat/modifiers/attribute_modifiers.hpp"
#include "gw2combat/modifiers/damage_modifiers.hpp"

#include "gw2combat/component/actor/destroy_after_rotation.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/rotation_component.hpp"
#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/actor/strike_counter.hpp"
#include "gw2combat/component/actor/team.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/attribute_modifiers_component.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/effect/is_unique_effect.hpp"
#include "gw2combat/component/filters.hpp"
#include "gw2combat/component/skill/ammo.hpp"
#include "gw2combat/component/skill/is_skill.hpp"
#include "gw2combat/component/trait/is_trait.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline component::attribute_modifiers_t convert_attribute_modifiers(
    const component::filters_t& filters,
    const modifiers::attribute_modifiers_t& attribute_modifiers) {
    return component::attribute_modifiers_t{
        .filters = filters,

        .power_addend = attribute_modifiers.power_addend,
        .precision_addend = attribute_modifiers.precision_addend,
        .toughness_addend = attribute_modifiers.toughness_addend,
        .vitality_addend = attribute_modifiers.vitality_addend,
        .concentration_addend = attribute_modifiers.concentration_addend,
        .condition_damage_addend = attribute_modifiers.condition_damage_addend,
        .expertise_addend = attribute_modifiers.expertise_addend,
        .ferocity_addend = attribute_modifiers.ferocity_addend,
        .healing_power_addend = attribute_modifiers.healing_power_addend,
        .armor_addend = attribute_modifiers.armor_addend,
        .max_health_addend = attribute_modifiers.max_health_addend,

        .boon_duration_pct_addend = attribute_modifiers.boon_duration_pct_addend,
        .critical_chance_pct_addend = attribute_modifiers.critical_chance_pct_addend,
        .critical_damage_pct_addend = attribute_modifiers.critical_damage_pct_addend,
        .condition_duration_pct_addend = attribute_modifiers.condition_duration_pct_addend,
        .burning_duration_pct_addend = attribute_modifiers.burning_duration_pct_addend,
        .bleeding_duration_pct_addend = attribute_modifiers.bleeding_duration_pct_addend,
        .confusion_duration_pct_addend = attribute_modifiers.confusion_duration_pct_addend,
        .poison_duration_pct_addend = attribute_modifiers.poison_duration_pct_addend,
        .torment_duration_pct_addend = attribute_modifiers.torment_duration_pct_addend,
    };
}

[[nodiscard]] static inline component::damage_modifiers_t convert_damage_modifiers(
    const component::filters_t& filters,
    const modifiers::damage_modifiers_t& damage_modifiers) {
    return component::damage_modifiers_t{
        .filters = filters,

        .strike_damage_multiplier = damage_modifiers.strike_damage_multiplier,
        .strike_damage_multiplier_add_group_addend =
            damage_modifiers.strike_damage_multiplier_add_group_addend,
        .condition_damage_multiplier = damage_modifiers.condition_damage_multiplier,
        .condition_damage_multiplier_add_group_addend =
            damage_modifiers.condition_damage_multiplier_add_group_addend,
        .burning_damage_multiplier = damage_modifiers.burning_damage_multiplier,
        .bleeding_damage_multiplier = damage_modifiers.bleeding_damage_multiplier,
        .confusion_damage_multiplier = damage_modifiers.confusion_damage_multiplier,
        .poison_damage_multiplier = damage_modifiers.poison_damage_multiplier,
        .torment_damage_multiplier = damage_modifiers.torment_damage_multiplier,

        .condition_duration_pct_addend = damage_modifiers.condition_duration_pct_addend,
        .burning_duration_pct_addend = damage_modifiers.burning_duration_pct_addend,
        .bleeding_duration_pct_addend = damage_modifiers.bleeding_duration_pct_addend,
        .confusion_duration_pct_addend = damage_modifiers.confusion_duration_pct_addend,
        .poison_duration_pct_addend = damage_modifiers.poison_duration_pct_addend,
        .torment_duration_pct_addend = damage_modifiers.torment_duration_pct_addend,
    };
}

[[nodiscard]] static inline component::static_attributes get_static_attributes_from_build(
    const actor::build_t& build) {
    component::static_attributes static_attributes;
    static_attributes.attribute_value_map[actor::attribute_t::POWER] = build.attributes.power;
    static_attributes.attribute_value_map[actor::attribute_t::PRECISION] =
        build.attributes.precision;
    static_attributes.attribute_value_map[actor::attribute_t::TOUGHNESS] =
        build.attributes.toughness;
    static_attributes.attribute_value_map[actor::attribute_t::VITALITY] = build.attributes.vitality;
    static_attributes.attribute_value_map[actor::attribute_t::CONCENTRATION] =
        build.attributes.concentration;
    static_attributes.attribute_value_map[actor::attribute_t::CONDITION_DAMAGE] =
        build.attributes.condition_damage;
    static_attributes.attribute_value_map[actor::attribute_t::EXPERTISE] =
        build.attributes.expertise;
    static_attributes.attribute_value_map[actor::attribute_t::FEROCITY] = build.attributes.ferocity;
    static_attributes.attribute_value_map[actor::attribute_t::HEALING_POWER] =
        build.attributes.healing_power;
    static_attributes.attribute_value_map[actor::attribute_t::ARMOR] = build.attributes.armor;
    static_attributes.attribute_value_map[actor::attribute_t::MAX_HEALTH] =
        build.attributes.max_health;
    static_attributes.attribute_value_map[actor::attribute_t::BOON_DURATION_PCT] =
        build.attributes.boon_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_PCT] =
        build.attributes.critical_chance_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CRITICAL_DAMAGE_PCT] =
        build.attributes.critical_damage_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CONDITION_DURATION_PCT] =
        build.attributes.condition_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::BURNING_DURATION_PCT] =
        build.attributes.burning_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::BLEEDING_DURATION_PCT] =
        build.attributes.bleeding_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CONFUSION_DURATION_PCT] =
        build.attributes.confusion_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::POISON_DURATION_PCT] =
        build.attributes.poison_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::TORMENT_DURATION_PCT] =
        build.attributes.torment_duration_pct;
    return static_attributes;
}

static inline entity_t add_skill_to_actor(actor::base_class_t base_class,
                                          const std::string& skill_name,
                                          entity_t actor_entity,
                                          registry_t& registry) {
    auto& skill_database =
        registry.get<actor::skill_database>(utils::get_owner(actor_entity, registry));
    auto skill_configuration = skill_database.find_by(actor::skill_t{base_class, skill_name});

    auto skill_entity = registry.create();
    registry.emplace<component::is_skill>(skill_entity,
                                          component::is_skill{skill_configuration.skill_key});
    registry.emplace<component::ammo>(
        skill_entity, component::ammo{skill_configuration.ammo, skill_configuration.ammo});
    registry.emplace<component::owner_actor>(
        skill_entity, component::owner_actor{utils::get_owner(actor_entity, registry)});

    auto& actor_skills_component =
        registry.get_or_emplace<component::skills_component>(actor_entity);
    actor_skills_component.skill_entities.emplace_back(
        component::skill_entity{skill_configuration.skill_key, skill_entity});

    if (skill_configuration.attribute_modifiers != modifiers::attribute_modifiers_t{}) {
        auto& attribute_modifiers_component =
            registry.emplace<component::attribute_modifiers_component>(skill_entity);
        attribute_modifiers_component.attribute_modifiers.emplace_back(convert_attribute_modifiers(
            component::filters_t{.source_skill = skill_configuration.skill_key},
            skill_configuration.attribute_modifiers));
    }
    if (skill_configuration.damage_modifiers != modifiers::damage_modifiers_t{}) {
        auto& outgoing_damage_modifiers_component =
            registry.emplace<component::damage_modifiers_component>(skill_entity);
        outgoing_damage_modifiers_component.damage_modifiers.emplace_back(convert_damage_modifiers(
            component::filters_t{.source_skill = skill_configuration.skill_key},
            skill_configuration.damage_modifiers));
    }

    registry.ctx().emplace_hint<std::string>(skill_entity, skill_name);

    return skill_entity;
}

static inline entity_t add_trait_to_actor(const actor::trait_t& trait,
                                          entity_t actor_entity,
                                          registry_t& registry) {
    auto& trait_database = actor::trait_database::instance();
    auto trait_configuration = trait_database.find_by(trait);

    auto trait_entity = registry.create();
    registry.emplace<component::is_trait>(trait_entity,
                                          component::is_trait{trait_configuration.trait_key});
    registry.emplace<component::owner_actor>(trait_entity, component::owner_actor{actor_entity});

    auto& actor_traits_component =
        registry.get_or_emplace<component::traits_component>(actor_entity);
    actor_traits_component.trait_entities.emplace_back(
        component::trait_entity{trait_configuration.trait_key, trait_entity});

    if (!trait_configuration.attribute_modifiers.empty()) {
        auto& attribute_modifiers_component =
            registry.emplace<component::attribute_modifiers_component>(trait_entity);
        for (auto& attribute_modifiers : trait_configuration.attribute_modifiers) {
            attribute_modifiers.filters.source_trait = trait_configuration.trait_key;
            attribute_modifiers_component.attribute_modifiers.emplace_back(attribute_modifiers);
        }
    }
    if (!trait_configuration.attribute_conversions.empty()) {
        auto& attribute_conversions_component =
            registry.emplace<component::attribute_conversions_component>(trait_entity);
        for (auto& attribute_conversions : trait_configuration.attribute_conversions) {
            attribute_conversions.filters.source_trait = trait_configuration.trait_key;
            attribute_conversions_component.attribute_conversions.emplace_back(
                attribute_conversions);
        }
    }
    if (!trait_configuration.damage_modifiers.empty()) {
        auto& damage_modifiers_component =
            registry.emplace<component::damage_modifiers_component>(trait_entity);
        for (auto& damage_modifier : trait_configuration.damage_modifiers) {
            damage_modifier.filters.source_trait = trait_configuration.trait_key;
            damage_modifiers_component.damage_modifiers.emplace_back(damage_modifier);
        }
    }
    if (!trait_configuration.skill_modifiers.empty()) {
        auto& skill_modifiers_component =
            registry.emplace<component::skill_modifiers_component>(trait_entity);
        for (auto& skill_modifier : trait_configuration.skill_modifiers) {
            skill_modifier.filters.source_trait = trait_configuration.trait_key;
            skill_modifiers_component.skill_modifiers.emplace_back(skill_modifier);
        }
    }

    registry.ctx().emplace_hint<std::string>(trait_entity, utils::to_string(trait));

    return trait_entity;
}

static inline std::optional<entity_t> add_unique_effect_to_actor(
    const actor::unique_effect_t& unique_effect,
    entity_t actor_entity,
    registry_t& registry) {
    auto& unique_effect_database = actor::unique_effect_database::instance();
    auto unique_effect_configuration = unique_effect_database.find_by(unique_effect);

    if (registry.any_of<component::unique_effects_component>(actor_entity) &&
        registry.get<component::unique_effects_component>(actor_entity).count(unique_effect) >=
            unique_effect_configuration.max_stored_stacks) {
        return std::nullopt;
    }

    auto unique_effect_entity = registry.create();
    registry.emplace<component::is_unique_effect>(
        unique_effect_entity,
        component::is_unique_effect{unique_effect_configuration.unique_effect_key});
    registry.emplace<component::owner_actor>(unique_effect_entity,
                                             component::owner_actor{actor_entity});

    auto& actor_unique_effects_component =
        registry.get_or_emplace<component::unique_effects_component>(actor_entity);
    actor_unique_effects_component.unique_effect_entities.emplace_back(
        component::unique_effect_entity{unique_effect_configuration.unique_effect_key,
                                        unique_effect_entity});

    if (!unique_effect_configuration.attribute_modifiers.empty()) {
        auto& attribute_modifiers_component =
            registry.emplace<component::attribute_modifiers_component>(unique_effect_entity);
        for (auto& attribute_modifiers : unique_effect_configuration.attribute_modifiers) {
            attribute_modifiers.filters.source_actor_unique_effect =
                unique_effect_configuration.unique_effect_key;
            attribute_modifiers_component.attribute_modifiers.emplace_back(attribute_modifiers);
        }
    }
    if (!unique_effect_configuration.attribute_conversions.empty()) {
        auto& attribute_conversions_component =
            registry.emplace<component::attribute_conversions_component>(unique_effect_entity);
        for (auto& attribute_conversions : unique_effect_configuration.attribute_conversions) {
            attribute_conversions.filters.source_actor_unique_effect =
                unique_effect_configuration.unique_effect_key;
            attribute_conversions_component.attribute_conversions.emplace_back(
                attribute_conversions);
        }
    }
    if (!unique_effect_configuration.damage_modifiers.empty()) {
        auto& damage_modifiers_component =
            registry.emplace<component::damage_modifiers_component>(unique_effect_entity);
        for (auto& damage_modifier : unique_effect_configuration.damage_modifiers) {
            damage_modifier.filters.source_actor_unique_effect =
                unique_effect_configuration.unique_effect_key;
            damage_modifiers_component.damage_modifiers.emplace_back(damage_modifier);
        }
    }
    if (!unique_effect_configuration.skill_triggers.empty()) {
        auto& skill_triggers_component =
            registry.emplace<component::skill_triggers_component>(unique_effect_entity);
        for (auto& skill_trigger : unique_effect_configuration.skill_triggers) {
            if (skill_trigger.filters.strike_counter_configuration) {
                registry.emplace<component::strike_counter>(
                    actor_entity,
                    component::strike_counter{
                        skill_trigger.filters.strike_counter_configuration->required_count});
            }
            skill_triggers_component.skill_triggers.emplace_back(skill_trigger);
            utils::add_skill_to_actor(
                skill_trigger.skill.base_class, skill_trigger.skill.name, actor_entity, registry);
        }
    }

    registry.ctx().emplace_hint<std::string>(unique_effect_entity, utils::to_string(unique_effect));

    return unique_effect_entity;
}

[[nodiscard]] static inline entity_t create_child_actor(entity_t parent_actor,
                                                        const std::string& name,
                                                        int team_id,
                                                        registry_t& registry) {
    auto child_actor = registry.create();
    registry.ctx().emplace_hint<std::string>(child_actor,
                                             fmt::format("actor{}-{}", child_actor, name));
    registry.emplace<component::is_actor>(child_actor);
    registry.emplace<component::owner_actor>(
        child_actor, component::owner_actor{utils::get_owner(parent_actor, registry)});
    registry.emplace<component::destroy_after_rotation>(child_actor);
    registry.emplace<component::team>(child_actor, component::team{team_id});
    return child_actor;
}

static inline void enqueue_child_skills(entity_t parent_actor,
                                        const std::string& child_name,
                                        const std::vector<actor::skill_t>& skills,
                                        registry_t& registry) {
    if (!skills.empty()) {
        auto child_actor = utils::create_child_actor(
            parent_actor, child_name, registry.get<component::team>(parent_actor).id, registry);
        // spdlog::info("[{}]: spawned {}",
        //              utils::get_current_tick(registry),
        //              utils::get_entity_name(child_actor, registry));
        actor::rotation_t rotation;
        for (const auto& skill : skills) {
            utils::add_skill_to_actor(skill.base_class, skill.name, child_actor, registry);
            rotation.skill_casts.emplace_back(actor::skill_cast_t{skill, 0});
        }
        registry.emplace<component::rotation_component>(
            child_actor, component::rotation_component{rotation, 0, 0, false});
    }
}

static inline void enqueue_child_skill(entity_t parent_actor,
                                       const actor::skill_t& skill,
                                       registry_t& registry) {
    auto child_actor = utils::create_child_actor(parent_actor,
                                                 skill.name + " Entity",
                                                 registry.get<component::team>(parent_actor).id,
                                                 registry);
    actor::rotation_t rotation;
    utils::add_skill_to_actor(skill.base_class, skill.name, child_actor, registry);
    rotation.skill_casts.emplace_back(actor::skill_cast_t{skill, 0});
    registry.emplace<component::rotation_component>(
        child_actor, component::rotation_component{rotation, 0, 0, false});
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ACTOR_UTILITIES_HPP
