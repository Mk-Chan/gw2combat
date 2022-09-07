#include "system.hpp"

#include "gw2combat/actor/attributes.hpp"
#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/actor/static_attributes.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/attribute_conversions_component.hpp"
#include "gw2combat/component/attribute_modifiers_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/owner_actor.hpp"
#include "gw2combat/component/skill/recharge.hpp"

namespace gw2combat::system {

void update_effective_attributes(const component::attribute_modifiers_t& attribute_modifiers,
                                 component::effective_attributes& effective_attributes) {
    effective_attributes.attribute_value_map[actor::attribute_t::POWER] +=
        attribute_modifiers.power_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::PRECISION] +=
        attribute_modifiers.precision_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::TOUGHNESS] +=
        attribute_modifiers.toughness_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::VITALITY] +=
        attribute_modifiers.vitality_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CONCENTRATION] +=
        attribute_modifiers.concentration_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CONDITION_DAMAGE] +=
        attribute_modifiers.condition_damage_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::EXPERTISE] +=
        attribute_modifiers.expertise_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::FEROCITY] +=
        attribute_modifiers.ferocity_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::HEALING_POWER] +=
        attribute_modifiers.healing_power_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::ARMOR] +=
        attribute_modifiers.armor_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::MAX_HEALTH] +=
        attribute_modifiers.max_health_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::BOON_DURATION_PCT] +=
        attribute_modifiers.boon_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_PCT] +=
        attribute_modifiers.critical_chance_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_DAMAGE_PCT] +=
        attribute_modifiers.critical_damage_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CONDITION_DURATION_PCT] +=
        attribute_modifiers.condition_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::BURNING_DURATION_PCT] +=
        attribute_modifiers.burning_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::BLEEDING_DURATION_PCT] +=
        attribute_modifiers.bleeding_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::CONFUSION_DURATION_PCT] +=
        attribute_modifiers.confusion_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::POISON_DURATION_PCT] +=
        attribute_modifiers.poison_duration_pct_addend;
    effective_attributes.attribute_value_map[actor::attribute_t::TORMENT_DURATION_PCT] +=
        attribute_modifiers.torment_duration_pct_addend;
}

inline bool filters_satisfied(const component::filters_t& filters,
                              entity_t source_entity,
                              registry_t& registry) {
    if (filters.source_skill_tag || filters.target_actor_effect ||
        filters.target_actor_unique_effect) {
        return false;
    }
    if (filters.rune) {
        bool is_satisfied =
            registry.any_of<component::rune_component>(source_entity) &&
            registry.get<component::rune_component>(source_entity).rune == *filters.rune;
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.weapon_type || filters.weapon_position || filters.weapon_sigil) {
        if (!registry.all_of<component::equipped_weapons, component::current_weapon_set>(
                source_entity)) {
            return false;
        }
        auto& equipped_weapons = registry.get<component::equipped_weapons>(source_entity);
        auto& current_weapon_set = registry.get<component::current_weapon_set>(source_entity);
        bool has_bundle_equipped = registry.any_of<component::bundle_component>(source_entity);
        bool is_satisfied = std::any_of(
            equipped_weapons.weapons.begin(),
            equipped_weapons.weapons.end(),
            [&](const actor::weapon& weapon) {
                return weapon.set == current_weapon_set.set &&
                       (!filters.weapon_type ||
                        (!has_bundle_equipped && weapon.type == *filters.weapon_type)) &&
                       (!filters.weapon_position ||
                        (!has_bundle_equipped && weapon.position == *filters.weapon_position)) &&
                       (!filters.weapon_sigil || weapon.sigil == *filters.weapon_sigil);
            });
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_trait) {
        if (!registry.any_of<component::traits_component>(source_entity)) {
            return false;
        }
        auto& trait_entities =
            registry.get<component::traits_component>(source_entity).trait_entities;
        bool is_satisfied = std::any_of(trait_entities.begin(),
                                        trait_entities.end(),
                                        [&](const component::trait_entity& trait_entity) {
                                            return trait_entity.trait == *filters.source_trait;
                                        });
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_actor_effect) {
        if (!registry.any_of<component::effects_component>(source_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::effects_component>(source_entity)
                                .has(*filters.source_actor_effect);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_skill) {
        if (!registry.any_of<component::skills_component>(source_entity)) {
            return false;
        }
        auto& skill_entities =
            registry.get<component::skills_component>(source_entity).skill_entities;
        bool is_satisfied =
            std::any_of(skill_entities.begin(),
                        skill_entities.end(),
                        [&](const component::skill_entity& skill_entity) {
                            return skill_entity.skill == *filters.source_skill &&
                                   !registry.any_of<component::cooldown, component::recharge>(
                                       skill_entity.entity);
                        });
        if (!is_satisfied) {
            return false;
        }
    }
    return true;
}

void calculate_effective_attributes(registry_t& registry) {
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_actor>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& effective_attributes = registry.emplace<component::effective_attributes>(entity);
            effective_attributes.attribute_value_map = static_attributes.attribute_value_map;
        });

    registry.view<component::owner_actor, component::attribute_modifiers_component>().each(
        [&](entity_t entity,
            const component::owner_actor& owner_actor,
            const component::attribute_modifiers_component& attribute_modifiers_component) {
            auto& effective_attributes =
                registry.get<component::effective_attributes>(owner_actor.entity);
            for (auto& attribute_modifiers : attribute_modifiers_component.attribute_modifiers) {
                if (filters_satisfied(attribute_modifiers.filters, owner_actor.entity, registry)) {
                    // spdlog::info("{} {} modifiers {}",
                    //              utils::get_entity_name(owner_actor.entity, registry),
                    //              utils::get_entity_name(entity, registry),
                    //              utils::to_string(attribute_modifiers));
                    update_effective_attributes(attribute_modifiers, effective_attributes);
                }
            }
        });

    registry.view<component::owner_actor, component::attribute_conversions_component>().each(
        [&](entity_t entity,
            const component::owner_actor& owner_actor,
            const component::attribute_conversions_component& attribute_conversions_component) {
            auto& effective_attributes =
                registry.get<component::effective_attributes>(owner_actor.entity);
            std::unordered_map<actor::attribute_t, double> attribute_conversion_bonuses;
            for (auto& attribute_conversions :
                 attribute_conversions_component.attribute_conversions) {
                if (filters_satisfied(
                        attribute_conversions.filters, owner_actor.entity, registry)) {
                    attribute_conversion_bonuses[attribute_conversions.to] +=
                        (int)(attribute_conversions.multiplier *
                                  effective_attributes
                                      .attribute_value_map[attribute_conversions.from] +
                              attribute_conversions.addend);
                }
            }
            for (auto&& [attribute, bonus] : attribute_conversion_bonuses) {
                effective_attributes.attribute_value_map[attribute] += bonus;
            }
        });

    registry.view<component::effective_attributes, component::effects_component>().each(
        [&](component::effective_attributes& effective_attributes,
            const component::effects_component& effects_component) {
            if (effects_component.has(effect::effect_t::FURY)) {
                effective_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_PCT] +=
                    25.0;
            }

            size_t might_stacks = effects_component.count(effect::effect_t::MIGHT);
            effective_attributes.attribute_value_map[actor::attribute_t::POWER] +=
                30.0 * (double)might_stacks;
            effective_attributes.attribute_value_map[actor::attribute_t::CONDITION_DAMAGE] +=
                30.0 * (double)might_stacks;
        });
}

}  // namespace gw2combat::system
