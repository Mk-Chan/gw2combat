#include "system.hpp"

#include "gw2combat/actor/unique_effect_database.hpp"

#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/damage/incoming_damage_modifiers.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/is_unique_effect.hpp"
#include "gw2combat/component/equipment/bundle.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/owner_actor.hpp"

namespace gw2combat::system {

inline bool filters_satisfied(const component::filters_t& filters,
                              entity_t source_entity,
                              entity_t target_entity,
                              registry_t& registry) {
    if (filters.source_skill || filters.source_skill_tag ||
        !(filters.target_actor_effect || filters.target_actor_unique_effect)) {
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
    if (filters.source_actor_unique_effect) {
        if (!registry.any_of<component::unique_effects_component>(source_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::unique_effects_component>(source_entity)
                                .has(*filters.source_actor_unique_effect);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.target_actor_effect) {
        if (!registry.any_of<component::effects_component>(target_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::effects_component>(target_entity)
                                .has(*filters.target_actor_effect);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.target_actor_unique_effect) {
        if (!registry.any_of<component::unique_effects_component>(target_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::unique_effects_component>(target_entity)
                                .has(*filters.target_actor_unique_effect);
        if (!is_satisfied) {
            return false;
        }
    }
    return true;
}

void calculate_incoming_damage_modifiers(registry_t& registry) {
    registry
        .view<component::is_actor, component::effects_component>(
            entt::exclude<component::owner_actor, component::incoming_damage_modifiers>)
        .each([&](entity_t target_entity,
                  const component::effects_component& target_effects_component) {
            auto& incoming_damage_modifiers =
                registry.emplace<component::incoming_damage_modifiers>(target_entity);
            incoming_damage_modifiers.source_entity_to_damage_modifiers[target_entity]
                .strike_damage_multiplier_add_group_addend +=
                0.01 * (double)target_effects_component.count(effect::effect_t::VULNERABILITY);
            incoming_damage_modifiers.source_entity_to_damage_modifiers[target_entity]
                .strike_damage_multiplier *=
                (1.0 - 0.33 * (double)target_effects_component.has(effect::effect_t::PROTECTION));
            incoming_damage_modifiers.source_entity_to_damage_modifiers[target_entity]
                .condition_damage_multiplier_add_group_addend +=
                0.01 * (double)target_effects_component.count(effect::effect_t::VULNERABILITY);
            incoming_damage_modifiers.source_entity_to_damage_modifiers[target_entity]
                .condition_damage_multiplier *=
                (1.0 - 0.33 * (double)target_effects_component.has(effect::effect_t::RESOLUTION));
            incoming_damage_modifiers.source_entity_to_attribute_modifiers[target_entity];
            {
                registry.view<component::is_actor>(entt::exclude<component::owner_actor>)
                    .each([&](entity_t entity) {
                        incoming_damage_modifiers.source_entity_to_damage_modifiers[entity];
                        incoming_damage_modifiers.source_entity_to_attribute_modifiers[entity];
                    });
            }
            {
                std::unordered_map<effect::effect_t, size_t> effect_to_considered_stacks_count;
                std::unordered_map<actor::unique_effect_t, size_t>
                    unique_effect_to_considered_stacks_count;
                registry.view<component::owner_actor, component::attribute_modifiers_component>()
                    .each([&](entity_t entity,
                              const component::owner_actor& owner_actor,
                              const component::attribute_modifiers_component&
                                  attribute_modifiers_component) {
                        if (registry.any_of<component::is_effect>(entity)) {
                            effect::effect_t this_effect =
                                registry.get<component::is_effect>(entity).effect;
                            if (effect_to_considered_stacks_count[this_effect] >=
                                utils::get_max_considered_stacks_of_effect_type(this_effect)) {
                                return;
                            }
                            ++effect_to_considered_stacks_count[this_effect];
                        }
                        if (registry.any_of<component::is_unique_effect>(entity)) {
                            actor::unique_effect_t this_unique_effect =
                                registry.get<component::is_unique_effect>(entity).unique_effect;
                            auto& unique_effect_configuration =
                                actor::unique_effect_database::instance().find_by(
                                    this_unique_effect);
                            if (unique_effect_to_considered_stacks_count[this_unique_effect] >=
                                unique_effect_configuration.max_considered_stacks) {
                                return;
                            }
                            ++unique_effect_to_considered_stacks_count[this_unique_effect];
                        }

                        for (auto& attribute_modifiers :
                             attribute_modifiers_component.attribute_modifiers) {
                            if (filters_satisfied(attribute_modifiers.filters,
                                                  owner_actor.entity,
                                                  target_entity,
                                                  registry)) {
                                // spdlog::info(
                                //     "incoming mod by_strikes {} by_effects {} target {} owner {}
                                //     " "entity {} attrmodifiers {}", filter_by_strikes,
                                //     filter_by_effects,
                                //     utils::get_entity_name(target_entity, registry),
                                //     utils::get_entity_name(owner_actor.entity, registry),
                                //     utils::get_entity_name(entity, registry),
                                //     utils::to_string(attribute_modifiers));
                                incoming_damage_modifiers
                                    .source_entity_to_attribute_modifiers[owner_actor.entity]
                                    .critical_chance_pct_addend +=
                                    attribute_modifiers.critical_chance_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_attribute_modifiers[owner_actor.entity]
                                    .critical_damage_pct_addend +=
                                    attribute_modifiers.critical_damage_pct_addend;
                            }
                        }
                    });
            }

            {
                std::unordered_map<effect::effect_t, size_t> effect_to_considered_stacks_count;
                std::unordered_map<actor::unique_effect_t, size_t>
                    unique_effect_to_considered_stacks_count;
                registry.view<component::owner_actor, component::damage_modifiers_component>().each(
                    [&](entity_t entity,
                        const component::owner_actor& owner_actor,
                        const component::damage_modifiers_component& damage_modifiers_component) {
                        if (registry.any_of<component::is_effect>(entity)) {
                            effect::effect_t this_effect =
                                registry.get<component::is_effect>(entity).effect;
                            if (effect_to_considered_stacks_count[this_effect] >=
                                utils::get_max_considered_stacks_of_effect_type(this_effect)) {
                                return;
                            }
                            ++effect_to_considered_stacks_count[this_effect];
                        }
                        if (registry.any_of<component::is_unique_effect>(entity)) {
                            actor::unique_effect_t this_unique_effect =
                                registry.get<component::is_unique_effect>(entity).unique_effect;
                            auto& unique_effect_configuration =
                                actor::unique_effect_database::instance().find_by(
                                    this_unique_effect);
                            if (unique_effect_to_considered_stacks_count[this_unique_effect] >=
                                unique_effect_configuration.max_considered_stacks) {
                                return;
                            }
                            ++unique_effect_to_considered_stacks_count[this_unique_effect];
                        }

                        for (auto& damage_modifiers : damage_modifiers_component.damage_modifiers) {
                            if (filters_satisfied(damage_modifiers.filters,
                                                  owner_actor.entity,
                                                  target_entity,
                                                  registry)) {
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .strike_damage_multiplier *=
                                    damage_modifiers.strike_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .strike_damage_multiplier_add_group_addend +=
                                    damage_modifiers.strike_damage_multiplier_add_group_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .condition_damage_multiplier *=
                                    damage_modifiers.condition_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .condition_damage_multiplier_add_group_addend +=
                                    damage_modifiers.condition_damage_multiplier_add_group_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .burning_damage_multiplier *=
                                    damage_modifiers.burning_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .bleeding_damage_multiplier *=
                                    damage_modifiers.bleeding_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .confusion_damage_multiplier *=
                                    damage_modifiers.confusion_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .poison_damage_multiplier *=
                                    damage_modifiers.poison_damage_multiplier;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .torment_damage_multiplier *=
                                    damage_modifiers.torment_damage_multiplier;

                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .condition_duration_pct_addend +=
                                    damage_modifiers.condition_duration_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .burning_duration_pct_addend +=
                                    damage_modifiers.burning_duration_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .bleeding_duration_pct_addend +=
                                    damage_modifiers.bleeding_duration_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .confusion_duration_pct_addend +=
                                    damage_modifiers.confusion_duration_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .poison_duration_pct_addend +=
                                    damage_modifiers.poison_duration_pct_addend;
                                incoming_damage_modifiers
                                    .source_entity_to_damage_modifiers[owner_actor.entity]
                                    .torment_duration_pct_addend +=
                                    damage_modifiers.torment_duration_pct_addend;
                                // spdlog::info(
                                //     "incoming mod by_strikes {} by_effects {} target {} owner {}
                                //     " "entity {} modifiers {} final {}", filter_by_strikes,
                                //     filter_by_effects,
                                //     utils::get_entity_name(target_entity, registry),
                                //     utils::get_entity_name(owner_actor.entity, registry),
                                //     utils::get_entity_name(entity, registry),
                                //     utils::to_string(damage_modifiers),
                                //     utils::to_string(incoming_damage_modifiers));
                            }
                        }
                    });
            }
        });
}

}  // namespace gw2combat::system
