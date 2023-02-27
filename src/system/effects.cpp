#include "effects.hpp"

#include "common.hpp"

#include "component/actor/effects_component.hpp"
#include "component/actor/is_actor.hpp"
#include "component/actor/unique_effects_component.hpp"
#include "component/damage/buffered_condition_damage.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/temporal/duration_component.hpp"

#include "system/attributes.hpp"

#include "utils/entity_utils.hpp"

namespace gw2combat::system {

double calculate_condition_damage(actor::effect_t this_effect,
                                  entity_t target_entity,
                                  const component::relative_attributes& source_relative_attributes,
                                  double base_condition_damage_multiplier) {
    switch (this_effect) {
        case actor::effect_t::BURNING:
            return (131.0 + 0.155 * source_relative_attributes.get(
                                        target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::BURNING_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::BLEEDING:
            return (22.0 + 0.06 * source_relative_attributes.get(
                                      target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::BLEEDING_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::TORMENT:
            // FIXME: This is stationary-only torment damage for golem logs
            return (31.8 + 0.09 * source_relative_attributes.get(
                                      target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::TORMENT_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::POISON:
            return (33.5 + 0.06 * source_relative_attributes.get(
                                      target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::POISON_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::CONFUSION:
            // FIXME: This is idle-only confusion damage for golem logs
            return (11.0 + 0.03 * source_relative_attributes.get(
                                      target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::CONFUSION_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::BINDING_BLADE:
            return (160.0 +
                    0.2 * source_relative_attributes.get(target_entity, actor::attribute_t::POWER));
        default:
            throw std::runtime_error(fmt::format("missing damage formula for effect_type: {}",
                                                 utils::to_string(this_effect)));
    }
}

void buffer_condition_damage(registry_t& registry,
                             actor::effect_t this_effect,
                             entity_t target_entity,
                             entity_t effect_entity,
                             entity_t effect_source_entity) {
    auto& source_relative_attributes =
        registry.get<component::relative_attributes>(effect_source_entity);
    auto& target_relative_attributes = registry.get<component::relative_attributes>(target_entity);

    double base_condition_damage_multiplier =
        source_relative_attributes.get(target_entity,
                                       actor::attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER) *
        (1.0 +
         source_relative_attributes.get(
             target_entity, actor::attribute_t::OUTGOING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP)) *
        target_relative_attributes.get(effect_source_entity,
                                       actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER) *
        (1.0 + target_relative_attributes.get(
                   effect_source_entity,
                   actor::attribute_t::INCOMING_CONDITION_DAMAGE_MULTIPLIER_ADD_GROUP));

    auto& condition_duration = registry.get<component::duration_component>(effect_entity);
    double damaging_condition_progress_multiplier = condition_duration.progress / 1'000.0;
    condition_duration.duration -= condition_duration.progress;
    condition_duration.progress = 0;
    double base_condition_damage = calculate_condition_damage(
        this_effect, target_entity, source_relative_attributes, base_condition_damage_multiplier);
    double effective_condition_damage =
        base_condition_damage * damaging_condition_progress_multiplier;

    auto& buffered_condition_damage =
        registry.get_or_emplace<component::buffered_condition_damage>(target_entity);
    buffered_condition_damage.source_entity_effect_tuple_to_value_map[std::make_tuple(
        effect_source_entity, this_effect)] += effective_condition_damage;

    double total_buffered_damage = std::accumulate(
        buffered_condition_damage.source_entity_effect_tuple_to_value_map.begin(),
        buffered_condition_damage.source_entity_effect_tuple_to_value_map.end(),
        0.0,
        [](double accumulated, const auto& entry) { return accumulated + entry.second; });
    spdlog::info(
        "[{}] {}:effect {} base_mult {} progress_mult {} base_dmg {} eff_dmg {} "
        "buffered_dmg {}",
        utils::get_current_tick(registry),
        utils::get_entity_name(target_entity, registry),
        utils::to_string(this_effect),
        base_condition_damage_multiplier,
        damaging_condition_progress_multiplier,
        base_condition_damage,
        effective_condition_damage,
        total_buffered_damage);
}

void buffer_condition_damage(registry_t& registry, std::optional<entity_t> specific_effect_entity) {
    system::calculate_relative_attributes(registry);

    if (specific_effect_entity) {
        actor::effect_t this_effect =
            registry.get<component::is_effect>(*specific_effect_entity).effect;
        if (!utils::is_damaging_condition(this_effect)) {
            return;
        }
        entity_t target_entity =
            registry.get<component::owner_component>(*specific_effect_entity).entity;
        entity_t effect_source_entity =
            registry.get<component::source_actor>(*specific_effect_entity).entity;
        buffer_condition_damage(
            registry, this_effect, target_entity, *specific_effect_entity, effect_source_entity);
    } else {
        registry.view<component::is_actor, component::effects_component>().each(
            [&](entity_t target_entity, const component::effects_component& effects_component) {
                for (auto effect_entity : effects_component.effect_entities) {
                    if (!utils::is_damaging_condition(effect_entity.effect)) {
                        continue;
                    }
                    actor::effect_t this_effect = effect_entity.effect;
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
            for (auto&& [source_entity_effect_tuple, buffered_damage] :
                 buffered_condition_damage.source_entity_effect_tuple_to_value_map) {
                incoming_damage.incoming_damage_events.emplace_back(
                    component::incoming_damage_event{utils::get_current_tick(registry),
                                                     std::get<0>(source_entity_effect_tuple),
                                                     std::get<1>(source_entity_effect_tuple),
                                                     "",
                                                     buffered_damage});
            }
            registry.remove<component::buffered_condition_damage>(entity);
        });
}

void buffer_damage_for_effects_with_no_duration(registry_t& registry) {
    registry.view<component::duration_expired, component::is_effect>().each(
        [&](entity_t entity, const component::is_effect& is_effect) {
            if (utils::is_damaging_condition(is_effect.effect)) {
                buffer_condition_damage(registry, entity);
            }
        });
}

void cleanup_expired_effects(registry_t& registry) {
    registry.view<component::effects_component>().each(
        [&](component::effects_component& effects_component) {
            for (auto iter = effects_component.effect_entities.begin();
                 iter != effects_component.effect_entities.end();) {
                if (!registry.valid(iter->entity)) {
                    iter = effects_component.effect_entities.erase(iter);
                } else {
                    ++iter;
                }
            }
        });
    registry.view<component::unique_effects_component>().each(
        [&](component::unique_effects_component& unique_effects_component) {
            for (auto iter = unique_effects_component.unique_effect_entities.begin();
                 iter != unique_effects_component.unique_effect_entities.end();) {
                if (!registry.valid(iter->entity)) {
                    iter = unique_effects_component.unique_effect_entities.erase(iter);
                } else {
                    ++iter;
                }
            }
        });
}

}  // namespace gw2combat::system
