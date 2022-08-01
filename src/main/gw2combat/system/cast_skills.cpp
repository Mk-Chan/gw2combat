#include "system.hpp"

#include "gw2combat/skills.hpp"

#include "gw2combat/component/character/destroy_after_rotation.hpp"
#include "gw2combat/component/character/is_actor.hpp"
#include "gw2combat/component/character/rotation.hpp"
#include "gw2combat/component/damage/multipliers/outgoing_strike_damage_multiplier.hpp"
#include "gw2combat/component/damage/outgoing_condition_application.hpp"
#include "gw2combat/component/damage/outgoing_strike_damage.hpp"
#include "gw2combat/component/effect_components.hpp"
#include "gw2combat/component/skills/instant_cast_skills.hpp"
#include "gw2combat/component/skills/normal_cast_skill.hpp"

namespace gw2combat::system {

void do_instant_cast_skill(registry_t& registry, entity_t entity, const skills::skill& skill) {
    if (!skill.hit_on_tick_list[0].empty()) {
        entity_t damage_source_entity = utils::get_damage_source_entity(entity, registry);
        auto& effective_attributes =
            registry.get<component::effective_attributes>(damage_source_entity);
        auto& outgoing_strike_damage_multiplier =
            registry.get<component::outgoing_strike_damage_multiplier>(damage_source_entity);

        double critical_hit_multiplier =
            (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                       (effective_attributes.critical_damage_pct / 100.0 - 1.0));

        auto& outgoing_strike_damage =
            registry.get_or_emplace<component::outgoing_strike_damage>(entity);
        outgoing_strike_damage.strikes.emplace_back(strike{
            entity, outgoing_strike_damage_multiplier.multiplier, critical_hit_multiplier, skill});
    }

    auto& child_entity_skills = skill.child_entity_skills;
    if (!child_entity_skills.empty()) {
        auto child_entity = registry.create();
        registry.emplace<component::is_actor>(child_entity);
        auto& rotation = registry.emplace<component::rotation>(child_entity);
        registry.emplace<component::destroy_after_rotation>(child_entity);
        registry.emplace<component::outgoing_damage_source>(
            child_entity,
            component::outgoing_damage_source{utils::get_damage_source_entity(entity, registry)});

        if (!child_entity_skills.empty()) {
            for (const auto& child_entity_skill : child_entity_skills) {
                rotation.skill_casts.emplace_back(skill_cast{0, child_entity_skill});
            }
        }
    }
}

void do_normal_cast_skill(registry_t& registry,
                          entity_t entity,
                          component::normal_cast_skill& normal_cast_skill) {
    bool has_quickness = registry.any_of<component::quickness>(entity);

    const skills::skill& skill = normal_cast_skill.skill;
    if (normal_cast_skill.next_hit_idx < skill.hit_on_tick_list[has_quickness].size() &&
        normal_cast_skill.progress >=
            skill.hit_on_tick_list[has_quickness][normal_cast_skill.next_hit_idx]) {
        entity_t damage_source_entity = utils::get_damage_source_entity(entity, registry);
        auto&& [effective_attributes, outgoing_strike_damage_multiplier] =
            registry
                .get<component::effective_attributes, component::outgoing_strike_damage_multiplier>(
                    damage_source_entity);
        auto& outgoing_strike_damage =
            registry.get_or_emplace<component::outgoing_strike_damage>(entity);
        double critical_hit_multiplier =
            (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                       (effective_attributes.critical_damage_pct / 100.0 - 1.0));
        outgoing_strike_damage.strikes.emplace_back(strike{
            entity, outgoing_strike_damage_multiplier.multiplier, critical_hit_multiplier, skill});

        ++normal_cast_skill.next_hit_idx;
    }

    if (normal_cast_skill.progress >= skill.cast_duration[has_quickness]) {
        auto& child_entity_skills = skill.child_entity_skills;
        if (!child_entity_skills.empty()) {
            auto child_entity = registry.create();
            registry.emplace<component::is_actor>(child_entity);
            auto& rotation = registry.emplace<component::rotation>(child_entity);
            registry.emplace<component::destroy_after_rotation>(child_entity);
            registry.emplace<component::outgoing_damage_source>(
                child_entity,
                component::outgoing_damage_source{
                    utils::get_damage_source_entity(entity, registry)});

            if (!child_entity_skills.empty()) {
                for (const auto& child_entity_skill : child_entity_skills) {
                    rotation.skill_casts.emplace_back(skill_cast{0, child_entity_skill});
                }
            }
        }

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
            registry.remove<component::instant_cast_skills>(entity);
        });
    registry.view<component::normal_cast_skill>().each(
        [&](entity_t entity, component::normal_cast_skill& normal_cast_skill) {
            do_normal_cast_skill(registry, entity, normal_cast_skill);
        });
}

}  // namespace gw2combat::system
