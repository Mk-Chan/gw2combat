#include "system.hpp"

#include "gw2combat/utilities/actor_utilities.hpp"
#include "gw2combat/utilities/damage_utilities.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"

#include "gw2combat/actor/skill_database.hpp"
#include "gw2combat/actor/unique_effect_database.hpp"

#include "gw2combat/component/actor/effective_attributes.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/actor/strike_counter.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/attribute_modifiers_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_damage_modifiers.hpp"
#include "gw2combat/component/damage/outgoing_damage_modifiers.hpp"
#include "gw2combat/component/damage/strikes_pipeline.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/is_unique_effect.hpp"
#include "gw2combat/component/equipment/weapons.hpp"
#include "gw2combat/component/owner_actor.hpp"
#include "gw2combat/component/skill/recharge.hpp"

namespace gw2combat::system {

inline bool filters_for_skill_trigger_satisfied(const component::filters_t& filters,
                                                entity_t source_entity,
                                                entity_t target_entity,
                                                const actor::skill_t& skill,
                                                registry_t& registry) {
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
    if (filters.source_skill) {
        bool is_satisfied = filters.source_skill == skill;
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_skill_tag) {
        auto& this_skill_configuration =
            registry.get<actor::skill_database>(utils::get_owner(source_entity, registry))
                .find_by(skill);
        bool is_satisfied = this_skill_configuration.tags.contains(*filters.source_skill_tag);
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
    if (filters.depends_on_skill_off_cooldown) {
        entity_t skill_entity = registry.get<component::skills_component>(source_entity)
                                    .find_by(*filters.depends_on_skill_off_cooldown);
        bool is_satisfied =
            !registry.any_of<component::cooldown, component::recharge>(skill_entity);
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.strike_counter_configuration) {
        if (!registry.any_of<component::strike_counter>(source_entity)) {
            return false;
        }
        const auto& strike_counter = registry.get<component::strike_counter>(source_entity);
        bool is_satisfied = strike_counter.value >= strike_counter.modulus;
        if (!is_satisfied) {
            return false;
        }
    }
    return true;
}

inline bool filters_for_strike_satisfied(const component::filters_t& filters,
                                         entity_t source_entity,
                                         entity_t target_entity,
                                         const strike::strike_t& this_strike,
                                         registry_t& registry) {
    if (!(filters.source_skill || filters.source_skill_tag)) {
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
    if (filters.source_skill) {
        bool is_satisfied = filters.source_skill == this_strike.skill;
        if (!is_satisfied) {
            return false;
        }
    }
    if (filters.source_skill_tag) {
        auto& this_skill_configuration =
            registry.get<actor::skill_database>(utils::get_owner(source_entity, registry))
                .find_by(this_strike.skill);
        bool is_satisfied = this_skill_configuration.tags.contains(*filters.source_skill_tag);
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

void apply_incoming_strikes(registry_t& registry) {
    registry
        .view<component::effective_attributes, component::incoming_strikes_component>(
            entt::exclude<component::owner_actor>)
        .each([&](entity_t target_entity,
                  const component::effective_attributes& effective_attributes,
                  const component::incoming_strikes_component& incoming_strikes_component) {
            calculate_outgoing_damage_modifiers(registry);
            calculate_incoming_damage_modifiers(registry);

            auto& target_incoming_damage_modifiers =
                registry.get<component::incoming_damage_modifiers>(target_entity);
            for (const auto& strike : incoming_strikes_component.strikes) {
                entity_t strike_source_entity = utils::get_owner(strike.source_entity, registry);

                double additional_critical_chance_pct_for_strike = 0.0;
                double additional_critical_damage_pct_for_strike = 0.0;
                std::unordered_map<effect::effect_t, size_t> effect_to_considered_stacks_count;
                std::unordered_map<actor::unique_effect_t, size_t>
                    unique_effect_to_considered_stacks_count;
                registry.view<component::owner_actor, component::attribute_modifiers_component>()
                    .each([&](entity_t entity,
                              const component::owner_actor& owner_actor,
                              const component::attribute_modifiers_component&
                                  attribute_modifiers_component) {
                        if (owner_actor.entity != strike_source_entity) {
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
                            if (attribute_modifiers.filters.target_actor_effect &&
                                filters_for_strike_satisfied(attribute_modifiers.filters,
                                                             strike_source_entity,
                                                             target_entity,
                                                             strike.strike,
                                                             registry)) {
                                // spdlog::info(
                                //     "INCOMING_STRIKE target {} owner {} entity {} attrmodifiers
                                //     {}", utils::get_entity_name(target_entity, registry),
                                //     utils::get_entity_name(owner_actor.entity, registry),
                                //     utils::get_entity_name(entity, registry),
                                //     utils::to_string(attribute_modifiers));
                                additional_critical_chance_pct_for_strike +=
                                    attribute_modifiers.critical_chance_pct_addend;
                                additional_critical_damage_pct_for_strike +=
                                    attribute_modifiers.critical_damage_pct_addend;
                            }
                        }
                    });

                double source_strike_based_damage_multiplier = 1.0;
                double source_strike_based_damage_multiplier_add_group_addend = 0.0;
                double target_strike_based_damage_multiplier = 1.0;
                double target_strike_based_damage_multiplier_add_group_addend = 0.0;
                std::unordered_map<entity_t, std::unordered_map<effect::effect_t, size_t>>
                    effect_to_considered_stacks;
                std::unordered_map<entity_t, std::unordered_map<actor::unique_effect_t, size_t>>
                    unique_effect_to_considered_stacks;
                registry.view<component::owner_actor, component::damage_modifiers_component>().each(
                    [&](entity_t entity,
                        const component::owner_actor& owner_actor,
                        const component::damage_modifiers_component& damage_modifiers_component) {
                        if (registry.any_of<component::is_effect>(entity)) {
                            effect::effect_t this_effect =
                                registry.get<component::is_effect>(entity).effect;
                            if (effect_to_considered_stacks[owner_actor.entity][this_effect] >=
                                utils::get_max_considered_stacks_of_effect_type(this_effect)) {
                                return;
                            }
                            ++effect_to_considered_stacks[owner_actor.entity][this_effect];
                        }
                        if (registry.any_of<component::is_unique_effect>(entity)) {
                            actor::unique_effect_t this_unique_effect =
                                registry.get<component::is_unique_effect>(entity).unique_effect;
                            auto& unique_effect_configuration =
                                actor::unique_effect_database::instance().find_by(
                                    this_unique_effect);
                            if (unique_effect_to_considered_stacks[owner_actor.entity]
                                                                  [this_unique_effect] >=
                                unique_effect_configuration.max_considered_stacks) {
                                return;
                            }
                            ++unique_effect_to_considered_stacks[owner_actor.entity]
                                                                [this_unique_effect];
                        }
                        for (auto& damage_modifiers : damage_modifiers_component.damage_modifiers) {
                            if (filters_for_strike_satisfied(damage_modifiers.filters,
                                                             strike_source_entity,
                                                             target_entity,
                                                             strike.strike,
                                                             registry)) {
                                // spdlog::info(
                                //     "INCOMING_STRIKE target {} owner {} entity {} dmgmodifiers
                                //     {}", utils::get_entity_name(target_entity, registry),
                                //     utils::get_entity_name(owner_actor.entity, registry),
                                //     utils::get_entity_name(entity, registry),
                                //     utils::to_string(damage_modifiers));
                                if (owner_actor.entity == strike_source_entity) {
                                    source_strike_based_damage_multiplier *=
                                        damage_modifiers.strike_damage_multiplier;
                                    source_strike_based_damage_multiplier_add_group_addend +=
                                        damage_modifiers.strike_damage_multiplier_add_group_addend;
                                } else {
                                    target_strike_based_damage_multiplier *=
                                        damage_modifiers.strike_damage_multiplier;
                                    target_strike_based_damage_multiplier_add_group_addend +=
                                        damage_modifiers.strike_damage_multiplier_add_group_addend;
                                }
                            }
                        }
                    });

                if (registry.any_of<component::strike_counter>(strike_source_entity)) {
                    auto& strike_counter =
                        registry.get<component::strike_counter>(strike_source_entity);
                    ++strike_counter.value;
                }

                registry.view<component::owner_actor, component::skill_triggers_component>().each(
                    [&](const component::owner_actor& owner_actor,
                        const component::skill_triggers_component& skill_triggers_component) {
                        if (owner_actor.entity != strike_source_entity) {
                            return;
                        }
                        for (auto& skill_trigger : skill_triggers_component.skill_triggers) {
                            if (filters_for_skill_trigger_satisfied(skill_trigger.filters,
                                                                    strike_source_entity,
                                                                    target_entity,
                                                                    strike.strike.skill,
                                                                    registry)) {
                                utils::enqueue_child_skill(
                                    strike_source_entity, skill_trigger.skill, registry);
                            }
                        }
                    });

                if (registry.any_of<component::strike_counter>(strike_source_entity)) {
                    auto& strike_counter =
                        registry.get<component::strike_counter>(strike_source_entity);
                    strike_counter.value %= strike_counter.modulus;
                }

                auto& outgoing_effects_component =
                    registry.get_or_emplace<component::outgoing_effects_component>(
                        strike_source_entity);
                std::copy(strike.strike.on_hit_effect_applications.begin(),
                          strike.strike.on_hit_effect_applications.end(),
                          std::back_inserter(outgoing_effects_component.effect_applications));

                auto& strike_source_outgoing_damage_modifiers =
                    registry.get<component::outgoing_damage_modifiers>(strike_source_entity);
                double total_strike_damage_multiplier =
                    (source_strike_based_damage_multiplier *
                     strike_source_outgoing_damage_modifiers.strike_damage_multiplier *
                     target_incoming_damage_modifiers.source_entity_to_damage_modifiers
                         .at(strike_source_entity)
                         .strike_damage_multiplier) *
                    (1.0 + source_strike_based_damage_multiplier_add_group_addend +
                     strike_source_outgoing_damage_modifiers
                         .strike_damage_multiplier_add_group_addend +
                     target_incoming_damage_modifiers.source_entity_to_damage_modifiers
                         .at(strike_source_entity)
                         .strike_damage_multiplier_add_group_addend) *
                    (target_strike_based_damage_multiplier *
                     target_incoming_damage_modifiers.source_entity_to_damage_modifiers
                         .at(target_entity)
                         .strike_damage_multiplier) *
                    (1.0 + target_strike_based_damage_multiplier_add_group_addend +
                     target_incoming_damage_modifiers.source_entity_to_damage_modifiers
                         .at(target_entity)
                         .strike_damage_multiplier_add_group_addend);
                double critical_hit_multiplier = utils::get_critical_hit_multiplier(
                    strike.strike.critical_chance_pct +
                        target_incoming_damage_modifiers.source_entity_to_attribute_modifiers
                            .at(strike_source_entity)
                            .critical_chance_pct_addend +
                        additional_critical_chance_pct_for_strike,
                    strike.strike.critical_damage_pct +
                        target_incoming_damage_modifiers.source_entity_to_attribute_modifiers
                            .at(strike_source_entity)
                            .critical_damage_pct_addend +
                        additional_critical_damage_pct_for_strike);

                auto& incoming_damage =
                    registry.get_or_emplace<component::incoming_damage>(target_entity);
                incoming_damage.value += strike.strike.base_outgoing_damage *
                                         critical_hit_multiplier * total_strike_damage_multiplier /
                                         effective_attributes[actor::attribute_t::ARMOR];
                spdlog::info(
                    "[{}] skill {} pow {} strike_mult {} crit_mult {} incoming_dmg {} "
                    "outgoing_stuff {}",
                    utils::get_current_tick(registry),
                    utils::to_string(strike.strike.skill),
                    registry.get<component::effective_attributes>(
                        strike_source_entity)[actor::attribute_t::POWER],
                    total_strike_damage_multiplier,
                    critical_hit_multiplier,
                    incoming_damage.value,
                    utils::to_string(strike.strike.on_hit_effect_applications));
            }
        });
}

}  // namespace gw2combat::system
