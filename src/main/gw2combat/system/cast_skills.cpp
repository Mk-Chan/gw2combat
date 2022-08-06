#include "system.hpp"

#include "gw2combat/skills.hpp"

#include "gw2combat/component/character/destroy_after_rotation.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/rotation.hpp"
#include "gw2combat/component/damage/incoming_condition_application.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/gear/bundles.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"

namespace gw2combat::system {

void do_weapon_swap(entity_t entity, registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        throw std::runtime_error("no equipped_weapon_set on entity");
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    if (current_set == weapon_set::SET_1) {
        registry.replace<component::equipped_weapon_set>(
            entity, component::equipped_weapon_set{weapon_set::SET_2});
    } else {
        assert(registry.get<component::equipped_weapon_set>(entity).current_set ==
               weapon_set::SET_2);
        registry.replace<component::equipped_weapon_set>(
            entity, component::equipped_weapon_set{weapon_set::SET_1});
    }
}

void do_pulse(registry_t& registry, entity_t entity, const skills::skill& skill) {
    assert(utils::get_source_entity(entity, registry) != entity ||
                   skill.weapon_type == weapon_type::EMPTY_HANDED ||
                   registry.any_of<component::bundle>(entity)
               ? true
               : utils::has_weapon_type(skill.weapon_type, entity, registry));

    entity_t source_entity = utils::get_source_entity(entity, registry);
    auto& effective_attributes = registry.get<component::effective_attributes>(source_entity);
    auto& outgoing_condition_application =
        registry.get_or_emplace<component::outgoing_condition_application>(entity);
    auto& incoming_condition_application =
        registry.get_or_emplace<component::incoming_condition_application>(entity);
    for (const auto& effect_application : skill.on_pulse_effect_applications) {
        if (effect_application.effect_direction == skills::applied_effect_direction::OUTGOING) {
            outgoing_condition_application.effect_applications.emplace_back(
                effects::effect_application{
                    effect_application.effect_type,
                    source_entity,
                    utils::get_effective_effect_duration(effect_application.duration,
                                                         effect_application.effect_type,
                                                         effective_attributes),
                    effect_application.num_stacks});
        } else if (effect_application.effect_direction ==
                   skills::applied_effect_direction::INCOMING) {
            incoming_condition_application.effect_applications.emplace_back(
                effects::effect_application{
                    effect_application.effect_type,
                    source_entity,
                    utils::get_effective_effect_duration(effect_application.duration,
                                                         effect_application.effect_type,
                                                         effective_attributes),
                    effect_application.num_stacks});
        }
    }
}

void do_strike(registry_t& registry, entity_t entity, const skills::skill& skill) {
    assert(utils::get_source_entity(entity, registry) != entity ||
                   skill.weapon_type == weapon_type::EMPTY_HANDED ||
                   registry.any_of<component::bundle>(entity)
               ? true
               : utils::has_weapon_type(skill.weapon_type, entity, registry));

    entity_t source_entity = utils::get_source_entity(entity, registry);
    auto&& [effective_attributes, outgoing_strike_damage_multiplier] =
        registry.get<component::effective_attributes, component::outgoing_strike_damage_multiplier>(
            source_entity);
    double critical_hit_multiplier = utils::get_critical_hit_multiplier(effective_attributes);
    auto& outgoing_strike_damage =
        registry.get_or_emplace<component::outgoing_strike_damage>(entity);
    outgoing_strike_damage.strikes.emplace_back(strike{
        entity, outgoing_strike_damage_multiplier.multiplier, critical_hit_multiplier, skill});
}

void do_child_skills(registry_t& registry, entity_t entity, const skills::skill& skill) {
    auto& child_entity_skills = skill.child_entity_skills;
    if (!child_entity_skills.empty()) {
        auto child_entity = registry.create();
        registry.emplace<component::is_actor>(child_entity);
        auto& rotation = registry.emplace<component::rotation>(child_entity);
        registry.emplace<component::destroy_after_rotation>(child_entity);
        registry.emplace<component::source_entity>(
            child_entity, component::source_entity{utils::get_source_entity(entity, registry)});

        if (!child_entity_skills.empty()) {
            for (const auto& child_entity_skill : child_entity_skills) {
                rotation.skill_casts.emplace_back(skill_cast{0, child_entity_skill});
            }
        }
    }
}

void do_instant_cast_skill(registry_t& registry, entity_t entity, const skills::skill& skill) {
    if (!skill.pulse_on_tick_list[0].empty()) {
        do_pulse(registry, entity, skill);
    }
    if (!skill.hit_on_tick_list[0].empty()) {
        do_strike(registry, entity, skill);
    }
    do_child_skills(registry, entity, skill);
}

void do_normal_cast_skill(registry_t& registry,
                          entity_t entity,
                          component::normal_cast_skill& normal_cast_skill) {
    bool has_quickness = utils::has_effect(effects::effect_type::QUICKNESS, entity, registry);

    const skills::skill& skill = normal_cast_skill.skill;
    while (normal_cast_skill.next_pulse_idx < skill.pulse_on_tick_list[has_quickness].size() &&
           normal_cast_skill.progress >=
               skill.pulse_on_tick_list[has_quickness][normal_cast_skill.next_pulse_idx]) {
        do_pulse(registry, entity, skill);
        ++normal_cast_skill.next_pulse_idx;
    }
    while (normal_cast_skill.next_hit_idx < skill.hit_on_tick_list[has_quickness].size() &&
           normal_cast_skill.progress >=
               skill.hit_on_tick_list[has_quickness][normal_cast_skill.next_hit_idx]) {
        do_strike(registry, entity, skill);
        ++normal_cast_skill.next_hit_idx;
    }
    if (normal_cast_skill.progress >= skill.cast_duration[has_quickness]) {
        if (normal_cast_skill.skill.name == "Weapon Swap") {
            do_weapon_swap(entity, registry);
        }
        do_child_skills(registry, entity, skill);
        registry.remove<component::normal_cast_skill>(entity);
        return;
    }
    ++normal_cast_skill.progress;
}

void cast_skills(registry_t& registry, tick_t current_tick) {
    registry.view<component::instant_cast_skills>().each(
        [&](entity_t entity, const component::instant_cast_skills& instant_cast_skills) {
            for (const skills::skill& skill : instant_cast_skills.skills) {
                do_instant_cast_skill(registry, entity, skill);
            }
        });
    registry.view<component::normal_cast_skill>().each(
        [&](entity_t entity, component::normal_cast_skill& normal_cast_skill) {
            do_normal_cast_skill(registry, entity, normal_cast_skill);
        });
}

}  // namespace gw2combat::system
