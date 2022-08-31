#include "system.hpp"

#include "gw2combat/utilities/effect_utilities.hpp"

#include "gw2combat/actor/skill_database.hpp"

#include "gw2combat/component/actor/effects_component.hpp"
#include "gw2combat/component/actor/is_actor.hpp"
#include "gw2combat/component/actor/unique_effects_component.hpp"
#include "gw2combat/component/damage/buffered_condition_damage.hpp"
#include "gw2combat/component/damage/incoming_damage.hpp"
#include "gw2combat/component/damage/incoming_damage_modifiers.hpp"
#include "gw2combat/component/damage/outgoing_damage_modifiers.hpp"
#include "gw2combat/component/effect/duration.hpp"
#include "gw2combat/component/effect/is_effect.hpp"
#include "gw2combat/component/effect/source_actor.hpp"
#include "gw2combat/component/owner_actor.hpp"

namespace gw2combat::system {

double calculate_condition_damage(
    effect::effect_t this_effect,
    const component::effective_attributes& effective_attributes,
    const component::outgoing_damage_modifiers& effect_source_outgoing_damage_modifiers,
    const modifiers::damage_modifiers_t& target_incoming_damage_modifiers,
    double base_condition_damage_multiplier) {
    switch (this_effect) {
        case effect::effect_t::BURNING:
            return (131.0 + 0.155 * effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   effect_source_outgoing_damage_modifiers.burning_damage_multiplier *
                   target_incoming_damage_modifiers.burning_damage_multiplier *
                   base_condition_damage_multiplier;
        case effect::effect_t::BLEEDING:
            return (22.0 + 0.06 * effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   effect_source_outgoing_damage_modifiers.bleeding_damage_multiplier *
                   target_incoming_damage_modifiers.bleeding_damage_multiplier *
                   base_condition_damage_multiplier;
        case effect::effect_t::TORMENT:
            // FIXME: This is stationary-only torment damage for golem logs only
            return (31.8 + 0.09 * effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   effect_source_outgoing_damage_modifiers.torment_damage_multiplier *
                   target_incoming_damage_modifiers.torment_damage_multiplier *
                   base_condition_damage_multiplier;
        case effect::effect_t::POISON:
            return (33.5 + 0.06 * effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   effect_source_outgoing_damage_modifiers.poison_damage_multiplier *
                   target_incoming_damage_modifiers.poison_damage_multiplier *
                   base_condition_damage_multiplier;
        case effect::effect_t::CONFUSION:
            // FIXME: This is idle-only confusion damage for golem logs only
            return (11.0 + 0.03 * effective_attributes[actor::attribute_t::CONDITION_DAMAGE]) *
                   effect_source_outgoing_damage_modifiers.confusion_damage_multiplier *
                   target_incoming_damage_modifiers.confusion_damage_multiplier *
                   base_condition_damage_multiplier;
        case effect::effect_t::BINDING_BLADE:
            return (160.0 + 0.2 * effective_attributes[actor::attribute_t::POWER]);
        default:
            throw std::runtime_error(fmt::format("missing damage formula for effect_type: {}",
                                                 utils::to_string(this_effect)));
    }
}

void buffer_condition_damage(registry_t& registry,
                             effect::effect_t this_effect,
                             entity_t target_entity,
                             entity_t effect_entity,
                             entity_t effect_source_entity) {
    auto& effect_source_outgoing_damage_modifiers =
        registry.get<component::outgoing_damage_modifiers>(effect_source_entity);
    auto& target_incoming_damage_modifiers =
        registry.get<component::incoming_damage_modifiers>(target_entity);
    double base_condition_damage_multiplier =
        (effect_source_outgoing_damage_modifiers.condition_damage_multiplier) *
        (1.0 +
         effect_source_outgoing_damage_modifiers.condition_damage_multiplier_add_group_addend) *
        (target_incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
             .condition_damage_multiplier) *
        (1.0 + target_incoming_damage_modifiers.source_entity_to_damage_modifiers.at(target_entity)
                   .condition_damage_multiplier_add_group_addend);

    auto& condition_duration = registry.get<component::duration>(effect_entity);
    double damaging_condition_progress_multiplier = condition_duration.progress / 1'000.0;
    condition_duration.duration -= condition_duration.progress;
    condition_duration.progress = 0;
    double base_condition_damage = calculate_condition_damage(
        this_effect,
        registry.get<component::effective_attributes>(effect_source_entity),
        effect_source_outgoing_damage_modifiers,
        target_incoming_damage_modifiers.source_entity_to_damage_modifiers[target_entity],
        base_condition_damage_multiplier);

    auto& buffered_condition_damage =
        registry.get_or_emplace<component::buffered_condition_damage>(target_entity);
    buffered_condition_damage.value +=
        damaging_condition_progress_multiplier * base_condition_damage;
    spdlog::info("[{}] effect {} base_mult {} progress_mult {} base_dmg {} buffered_dmg {}",
                 utils::get_current_tick(registry),
                 utils::to_string(this_effect),
                 base_condition_damage_multiplier,
                 damaging_condition_progress_multiplier,
                 base_condition_damage,
                 buffered_condition_damage.value);
}

void buffer_condition_damage(registry_t& registry, std::optional<entity_t> specific_effect_entity) {
    if (specific_effect_entity) {
        effect::effect_t this_effect =
            registry.get<component::is_effect>(*specific_effect_entity).effect;
        if (!utils::is_damaging_condition(this_effect)) {
            return;
        }
        calculate_outgoing_damage_modifiers(registry);
        calculate_incoming_damage_modifiers(registry);
        entity_t target_entity =
            registry.get<component::owner_actor>(*specific_effect_entity).entity;
        entity_t effect_source_entity =
            registry.get<component::source_actor>(*specific_effect_entity).entity;
        buffer_condition_damage(
            registry, this_effect, target_entity, *specific_effect_entity, effect_source_entity);
    } else {
        registry.view<component::is_actor, component::effects_component>().each(
            [&](entity_t target_entity, const component::effects_component& effects_component) {
                calculate_outgoing_damage_modifiers(registry);
                calculate_incoming_damage_modifiers(registry);
                for (auto effect_entity : effects_component.effect_entities) {
                    if (!utils::is_damaging_condition(effect_entity.effect)) {
                        continue;
                    }
                    effect::effect_t this_effect = effect_entity.effect;
                    entity_t effect_source_entity =
                        registry.get<component::source_actor>(effect_entity.entity).entity;
                    buffer_condition_damage(registry,
                                            this_effect,
                                            target_entity,
                                            effect_entity.entity,
                                            effect_source_entity);
                }
            });
    }
}

void apply_condition_damage(registry_t& registry) {
    registry.view<component::buffered_condition_damage>().each(
        [&](entity_t entity,
            const component::buffered_condition_damage& buffered_condition_damage) {
            auto& incoming_damage = registry.get_or_emplace<component::incoming_damage>(entity);
            incoming_damage.value += buffered_condition_damage.value;
            registry.remove<component::buffered_condition_damage>(entity);
        });
}

}  // namespace gw2combat::system
