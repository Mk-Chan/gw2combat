#include "system.hpp"

#include "gw2combat/utilities/entity_utilities.hpp"

#include "gw2combat/actor/unique_effect_database.hpp"

#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/outgoing_damage_modifiers.hpp"
#include "gw2combat/component/damage/strikes_pipeline.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/is_unique_effect.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/owner_actor.hpp"

namespace gw2combat::system {

inline bool filters_satisfied(const component::filters_t& filters,
                              entity_t source_entity,
                              registry_t& registry) {
    if (filters.target_actor_unique_effect || filters.target_actor_effect) {
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
        bool is_satisfied = std::any_of(
            equipped_weapons.weapons.begin(),
            equipped_weapons.weapons.end(),
            [&](const actor::weapon& weapon) {
                return weapon.set == current_weapon_set.set &&
                       (!filters.weapon_type || weapon.type == *filters.weapon_type) &&
                       (!filters.weapon_position || weapon.position == *filters.weapon_position) &&
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
    return true;
}

void calculate_outgoing_damage_modifiers(registry_t& registry) {
    registry
        .view<component::is_actor>(
            entt::exclude<component::owner_actor, component::outgoing_damage_modifiers>)
        .each([&](entity_t source_entity) {
            auto& outgoing_damage_modifiers =
                registry.emplace<component::outgoing_damage_modifiers>(source_entity);

            std::unordered_map<effect::effect_t, size_t> effect_to_considered_stacks_count;
            std::unordered_map<actor::unique_effect_t, size_t>
                unique_effect_to_considered_stacks_count;
            registry.view<component::owner_actor, component::damage_modifiers_component>().each(
                [&](entity_t entity,
                    const component::owner_actor& owner_actor,
                    const component::damage_modifiers_component& damage_modifiers_component) {
                    if (owner_actor.entity != source_entity) {
                        return;
                    }
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
                            actor::unique_effect_database::instance().find_by(this_unique_effect);
                        if (unique_effect_to_considered_stacks_count[this_unique_effect] >=
                            unique_effect_configuration.max_considered_stacks) {
                            return;
                        }
                        ++unique_effect_to_considered_stacks_count[this_unique_effect];
                    }

                    for (auto& damage_modifiers : damage_modifiers_component.damage_modifiers) {
                        if (filters_satisfied(
                                damage_modifiers.filters, owner_actor.entity, registry)) {
                            // spdlog::info("outgoing mod source {} owner {} entity {} modifiers
                            // {}",
                            //              utils::get_entity_name(source_entity, registry),
                            //              utils::get_entity_name(owner_actor.entity, registry),
                            //              utils::get_entity_name(entity, registry),
                            //              utils::to_string(damage_modifiers));
                            outgoing_damage_modifiers.strike_damage_multiplier *=
                                damage_modifiers.strike_damage_multiplier;
                            outgoing_damage_modifiers.strike_damage_multiplier_add_group_addend +=
                                damage_modifiers.strike_damage_multiplier_add_group_addend;
                            outgoing_damage_modifiers.condition_damage_multiplier *=
                                damage_modifiers.condition_damage_multiplier;
                            outgoing_damage_modifiers
                                .condition_damage_multiplier_add_group_addend +=
                                damage_modifiers.condition_damage_multiplier_add_group_addend;
                            outgoing_damage_modifiers.burning_damage_multiplier *=
                                damage_modifiers.burning_damage_multiplier;
                            outgoing_damage_modifiers.bleeding_damage_multiplier *=
                                damage_modifiers.bleeding_damage_multiplier;
                            outgoing_damage_modifiers.confusion_damage_multiplier *=
                                damage_modifiers.confusion_damage_multiplier;
                            outgoing_damage_modifiers.poison_damage_multiplier *=
                                damage_modifiers.poison_damage_multiplier;
                            outgoing_damage_modifiers.torment_damage_multiplier *=
                                damage_modifiers.torment_damage_multiplier;

                            outgoing_damage_modifiers.condition_duration_pct_addend +=
                                damage_modifiers.condition_duration_pct_addend;
                            outgoing_damage_modifiers.burning_duration_pct_addend +=
                                damage_modifiers.burning_duration_pct_addend;
                            outgoing_damage_modifiers.bleeding_duration_pct_addend +=
                                damage_modifiers.bleeding_duration_pct_addend;
                            outgoing_damage_modifiers.confusion_duration_pct_addend +=
                                damage_modifiers.confusion_duration_pct_addend;
                            outgoing_damage_modifiers.poison_duration_pct_addend +=
                                damage_modifiers.poison_duration_pct_addend;
                            outgoing_damage_modifiers.torment_duration_pct_addend +=
                                damage_modifiers.torment_duration_pct_addend;
                        }
                    }
                });
        });
}

}  // namespace gw2combat::system
