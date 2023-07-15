#include "effects.hpp"

#include "common.hpp"

#include "component/actor/is_actor.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/damage/buffered_condition_damage.hpp"
#include "component/damage/incoming_damage.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/effect/source_skill.hpp"
#include "component/hierarchy/owner_component.hpp"
#include "component/temporal/duration_component.hpp"

#include "system/attributes.hpp"

#include "utils/effect_utils.hpp"
#include "utils/entity_utils.hpp"
#include "utils/skill_utils.hpp"

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
            return (18.25 + 0.05 * source_relative_attributes.get(
                                       target_entity, actor::attribute_t::CONDITION_DAMAGE)) *
                   source_relative_attributes.get(target_entity,
                                                  actor::attribute_t::CONFUSION_DAMAGE_MULTIPLIER) *
                   base_condition_damage_multiplier;
        case actor::effect_t::BINDING_BLADE:
            return (160.0 +
                    0.3 * source_relative_attributes.get(target_entity, actor::attribute_t::POWER));
        default:
            throw std::runtime_error(fmt::format("missing damage formula for effect_type: {}",
                                                 utils::to_string(this_effect)));
    }
}

void buffer_condition_damage(registry_t& registry,
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
    auto& is_effect = registry.get<component::is_effect>(effect_entity);
    auto& this_effect = is_effect.effect;
    double base_condition_damage = calculate_condition_damage(
        this_effect, target_entity, source_relative_attributes, base_condition_damage_multiplier);
    double effective_condition_damage =
        utils::round_to_nearest_even(base_condition_damage *
                                     damaging_condition_progress_multiplier *
                                     is_effect.grouped_with_num_stacks) /
        (double)is_effect.grouped_with_num_stacks;

    auto& buffered_condition_damage =
        registry.get_or_emplace<component::buffered_condition_damage>(target_entity);
    buffered_condition_damage.condition_damage_buffer.emplace_back(component::condition_damage_t{
        .effect_source_entity = effect_source_entity,
        .effect = this_effect,
        .source_skill = registry.get<component::source_skill>(effect_entity).skill,
        .damage = effective_condition_damage});

    // double total_buffered_damage = std::accumulate(
    //     buffered_condition_damage.condition_damage_buffer.begin(),
    //     buffered_condition_damage.condition_damage_buffer.end(),
    //     0.0,
    //     [](double accumulated, const component::condition_damage_t& condition_damage) {
    //         return accumulated + condition_damage.damage;
    //     });
    // spdlog::info(
    //     "[{}] {}:effect {}:{} condi_dmg {} base_mult {} progress_mult {} base_dmg {} eff_dmg {} "
    //     "buffered_dmg {}",
    //     utils::get_current_tick(registry),
    //     utils::get_entity_name(target_entity, registry),
    //     registry.get<component::source_skill>(effect_entity).skill,
    //     utils::to_string(this_effect),
    //     registry.get<component::relative_attributes>(effect_source_entity)
    //         .get(target_entity, actor::attribute_t::CONDITION_DAMAGE),
    //     base_condition_damage_multiplier,
    //     damaging_condition_progress_multiplier,
    //     base_condition_damage,
    //     effective_condition_damage,
    //     utils::round_to_nearest_even(total_buffered_damage));
}

void buffer_condition_damage(registry_t& registry, std::optional<entity_t> specific_effect_entity) {
    system::calculate_relative_attributes(registry);

    if (specific_effect_entity) {
        if (!registry.any_of<component::is_damaging_effect>(*specific_effect_entity)) {
            return;
        }
        entity_t target_entity =
            registry.get<component::owner_component>(*specific_effect_entity).entity;
        entity_t effect_source_entity =
            registry.get<component::source_actor>(*specific_effect_entity).entity;
        buffer_condition_damage(
            registry, target_entity, *specific_effect_entity, effect_source_entity);
    } else {
        registry
            .view<component::is_damaging_effect,
                  component::owner_component,
                  component::source_actor>()
            .each([&](entity_t effect_entity,
                      const component::owner_component& owner_component,
                      const component::source_actor& source_actor) {
                buffer_condition_damage(
                    registry, owner_component.entity, effect_entity, source_actor.entity);
            });
    }
}

void apply_condition_damage(registry_t& registry) {
    registry.view<component::buffered_condition_damage>().each(
        [&](entity_t entity,
            const component::buffered_condition_damage& buffered_condition_damage) {
            auto& incoming_damage = registry.get_or_emplace<component::incoming_damage>(entity);
            for (auto& condition_damage : buffered_condition_damage.condition_damage_buffer) {
                incoming_damage.incoming_damage_events.emplace_back(
                    component::incoming_damage_event{utils::get_current_tick(registry),
                                                     condition_damage.effect_source_entity,
                                                     condition_damage.effect,
                                                     condition_damage.source_skill,
                                                     condition_damage.damage});
            }
            registry.remove<component::buffered_condition_damage>(entity);
        });
}

void buffer_damage_for_effects_with_no_duration(registry_t& registry) {
    registry.view<component::duration_expired, component::is_damaging_effect>().each(
        [&](entity_t entity) { buffer_condition_damage(registry, entity); });
}

}  // namespace gw2combat::system
