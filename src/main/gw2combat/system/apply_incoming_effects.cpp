#include "system.hpp"

#include "gw2combat/utilities/actor_utilities.hpp"
#include "gw2combat/utilities/entity_utilities.hpp"

#include "gw2combat/component/actor/alacrity.hpp"
#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/quickness.hpp"
#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/actor/skills_component.hpp"
#include "gw2combat/component/actor/traits_component.hpp"
#include "gw2combat/component/cooldown.hpp"
#include "gw2combat/component/damage/effects_pipeline.hpp"
#include "gw2combat/component/damage/incoming_damage_modifiers.hpp"
#include "gw2combat/component/effect/duration.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/source_actor.hpp"
#include "gw2combat/component/equipment/weapons.hpp"

namespace gw2combat::system {

inline bool filters_satisfied(const component::filters_t& filters,
                              entity_t source_entity,
                              entity_t target_entity,
                              registry_t& registry) {
    if (filters.source_skill_tag) {
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
                                   !registry.any_of<component::cooldown>(skill_entity.entity);
                        });
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
    return true;
}

void apply_incoming_effects(registry_t& registry) {
    registry
        .view<component::effects_component,
              component::effective_attributes,
              component::incoming_effects_component>(entt::exclude<component::owner_actor>)
        .each([&](entity_t target_entity,
                  component::effects_component& effects_component,
                  const component::effective_attributes& effective_attributes,
                  const component::incoming_effects_component& incoming_effects_component) {
            calculate_incoming_damage_modifiers(registry);
            auto& incoming_damage_modifiers =
                registry.get<component::incoming_damage_modifiers>(target_entity);

            utils::effect_duration_pct_addends_t self_effect_duration_pct_addends;
            if (incoming_damage_modifiers.source_entity_to_damage_modifiers.contains(
                    target_entity)) {
                self_effect_duration_pct_addends.condition_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .condition_duration_pct_addend;
                self_effect_duration_pct_addends.burning_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .burning_duration_pct_addend;
                self_effect_duration_pct_addends.bleeding_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .bleeding_duration_pct_addend;
                self_effect_duration_pct_addends.poison_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .poison_duration_pct_addend;
                self_effect_duration_pct_addends.confusion_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .confusion_duration_pct_addend;
                self_effect_duration_pct_addends.torment_duration_pct_addend +=
                    incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                        .torment_duration_pct_addend;
            }
            for (const auto& incoming_application :
                 incoming_effects_component.effect_applications) {
                const auto& application = incoming_application.effect_application;
                entity_t application_source =
                    utils::get_owner(incoming_application.source_entity, registry);

                utils::effect_duration_pct_addends_t application_source_effect_duration_pct_addends;
                application_source_effect_duration_pct_addends.boon_duration_pct_addend +=
                    self_effect_duration_pct_addends.boon_duration_pct_addend;
                application_source_effect_duration_pct_addends.condition_duration_pct_addend +=
                    self_effect_duration_pct_addends.condition_duration_pct_addend;
                application_source_effect_duration_pct_addends.burning_duration_pct_addend +=
                    self_effect_duration_pct_addends.burning_duration_pct_addend;
                application_source_effect_duration_pct_addends.bleeding_duration_pct_addend +=
                    self_effect_duration_pct_addends.bleeding_duration_pct_addend;
                application_source_effect_duration_pct_addends.poison_duration_pct_addend +=
                    self_effect_duration_pct_addends.poison_duration_pct_addend;
                application_source_effect_duration_pct_addends.confusion_duration_pct_addend +=
                    self_effect_duration_pct_addends.confusion_duration_pct_addend;
                application_source_effect_duration_pct_addends.torment_duration_pct_addend +=
                    self_effect_duration_pct_addends.torment_duration_pct_addend;

                for (auto&& [entity, damage_modifiers] :
                     incoming_damage_modifiers.source_entity_to_damage_modifiers) {
                    if (entity != application_source) {
                        continue;
                    }
                    application_source_effect_duration_pct_addends.condition_duration_pct_addend +=
                        damage_modifiers.condition_duration_pct_addend;
                    application_source_effect_duration_pct_addends.burning_duration_pct_addend +=
                        damage_modifiers.burning_duration_pct_addend;
                    application_source_effect_duration_pct_addends.bleeding_duration_pct_addend +=
                        damage_modifiers.bleeding_duration_pct_addend;
                    application_source_effect_duration_pct_addends.poison_duration_pct_addend +=
                        damage_modifiers.poison_duration_pct_addend;
                    application_source_effect_duration_pct_addends.confusion_duration_pct_addend +=
                        damage_modifiers.confusion_duration_pct_addend;
                    application_source_effect_duration_pct_addends.torment_duration_pct_addend +=
                        damage_modifiers.torment_duration_pct_addend;
                }

                int effective_duration = utils::get_effective_effect_duration(
                    application.base_duration,
                    application.effect_type,
                    effective_attributes,
                    application_source_effect_duration_pct_addends);
                for (int num_stacks = application.num_stacks; num_stacks > 0; --num_stacks) {
                    if (application.unique_effect_type != actor::unique_effect_t{}) {
                        auto unique_effect_entity = utils::add_unique_effect_to_actor(
                            application.unique_effect_type, target_entity, registry);
                        if (unique_effect_entity) {
                            registry.emplace<component::duration>(
                                *unique_effect_entity, component::duration{effective_duration});
                        }
                    }

                    auto effect_entity = registry.create();
                    registry.emplace<component::is_effect>(
                        effect_entity, component::is_effect{application.effect_type});
                    registry.emplace<component::owner_actor>(effect_entity,
                                                             component::owner_actor{target_entity});
                    registry.emplace<component::source_actor>(
                        effect_entity, component::source_actor{application_source});
                    effects_component.effect_entities.emplace_back(
                        component::effect_entity{application.effect_type, effect_entity});
                    registry.emplace<component::duration>(effect_entity,
                                                          component::duration{effective_duration});
                    if (application.effect_type == effect::effect_t::QUICKNESS) {
                        registry.emplace_or_replace<component::quickness>(target_entity);
                    } else if (application.effect_type == effect::effect_t::ALACRITY) {
                        registry.emplace_or_replace<component::alacrity>(target_entity);
                    }

                    registry.ctx().emplace_hint<std::string>(
                        effect_entity,
                        utils::get_entity_name(target_entity, registry) + "-" +
                            utils::to_string(application.effect_type) + " Entity");
                }
            }
        });
}

}  // namespace gw2combat::system
