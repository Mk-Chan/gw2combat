#include "attributes.hpp"

#include "utils/condition_utils.hpp"
#include "utils/effect_utils.hpp"
#include "utils/entity_utils.hpp"

#include "component/actor/is_actor.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/actor/static_attributes.hpp"
#include "component/attributes/is_attribute_conversion.hpp"
#include "component/attributes/is_attribute_modifier.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/hierarchy/owner_component.hpp"

namespace gw2combat::system {

void calculate_relative_attributes(registry_t& registry) {
    if (!registry.view<component::relative_attributes>().empty()) {
        return;
    }
    registry
        .view<component::is_actor, component::static_attributes>(
            entt::exclude<component::owner_component, component::relative_attributes>)
        .each([&](entity_t entity, const component::static_attributes& static_attributes) {
            auto& relative_attributes = registry.emplace<component::relative_attributes>(entity);
            registry
                .view<component::is_actor, component::static_attributes>(
                    entt::exclude<component::owner_component>)
                .each([&](entity_t other_actor, const component::static_attributes&) {
                    for (auto& [attribute, value] : static_attributes.attribute_value_map) {
                        relative_attributes.set(other_actor, attribute, value);
                    }
                });
        });

    std::map<std::tuple<actor::unique_effect_t, entity_t, entity_t>, int>
        modifier_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map;
    std::map<std::tuple<actor::effect_t, entity_t, entity_t>, int>
        modifier_effect_and_owner_actor_and_other_actor_to_occurrences_map;
    registry.view<component::owner_component, component::is_attribute_modifier>().each(
        [&](const component::owner_component& owner_component,
            const component::is_attribute_modifier& is_attribute_modifier) {
            auto unique_effect_modifier_ptr =
                registry.try_get<component::is_unique_effect>(owner_component.entity);
            auto effect_modifier_ptr =
                registry.try_get<component::is_effect>(owner_component.entity);
            auto owner_actor = utils::get_owner(owner_component.entity, registry);
            auto& relative_attributes = registry.get<component::relative_attributes>(owner_actor);
            registry.view<component::relative_attributes>().each([&](entity_t other_actor,
                                                                     const component::
                                                                         relative_attributes&) {
                if (unique_effect_modifier_ptr) {
                    if (modifier_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_modifier_ptr->unique_effect.unique_effect_key,
                                owner_actor,
                                other_actor)] >=
                        unique_effect_modifier_ptr->unique_effect.max_considered_stacks) {
                        return;
                    } else {
                        ++modifier_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_modifier_ptr->unique_effect.unique_effect_key,
                                owner_actor,
                                other_actor)];
                    }
                }
                if (effect_modifier_ptr) {
                    if (modifier_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                effect_modifier_ptr->effect, owner_actor, other_actor)] >=
                        utils::get_max_considered_stacks_of_effect_type(
                            effect_modifier_ptr->effect)) {
                        return;
                    } else {
                        ++modifier_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                effect_modifier_ptr->effect, owner_actor, other_actor)];
                    }
                }
                for (auto& attribute_modifier : is_attribute_modifier.attribute_modifiers) {
                    if (utils::independent_conditions_satisfied(
                            attribute_modifier.condition, owner_actor, other_actor, registry)) {
                        relative_attributes.set(
                            other_actor,
                            attribute_modifier.attribute,
                            relative_attributes.get(other_actor, attribute_modifier.attribute) *
                                    attribute_modifier.multiplier +
                                attribute_modifier.addend);
                        // spdlog::info(
                        //     "{}:{} source {} modifier {} final {}",
                        //     utils::get_entity_name(owner_actor, registry),
                        //     utils::get_entity_name(other_actor, registry),
                        //     utils::get_entity_name(owner_component.entity, registry),
                        //     utils::to_string(attribute_modifier),
                        //     relative_attributes.get(other_actor, attribute_modifier.attribute));
                    }
                }
            });
        });

    std::map<std::tuple<actor::unique_effect_t, entity_t, entity_t>, int>
        conversion_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map;
    std::map<std::tuple<actor::effect_t, entity_t, entity_t>, int>
        conversion_effect_and_owner_actor_and_other_actor_to_occurrences_map;
    registry.view<component::relative_attributes>().each([&](entity_t other_actor,
                                                             const component::
                                                                 relative_attributes&) {
        std::map<std::tuple<entity_t, actor::attribute_t>, double>
            owner_actor_to_attribute_conversion_bonuses;
        registry.view<component::owner_component, component::is_attribute_conversion>().each(
            [&](const component::owner_component& owner_component,
                const component::is_attribute_conversion& is_attribute_conversion) {
                auto unique_effect_conversion_ptr =
                    registry.try_get<component::is_unique_effect>(owner_component.entity);
                auto effect_conversion_ptr =
                    registry.try_get<component::is_effect>(owner_component.entity);
                auto owner_actor = utils::get_owner(owner_component.entity, registry);
                auto& relative_attributes =
                    registry.get<component::relative_attributes>(owner_actor);
                if (unique_effect_conversion_ptr) {
                    if (conversion_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_conversion_ptr->unique_effect.unique_effect_key,
                                owner_actor,
                                other_actor)] >=
                        unique_effect_conversion_ptr->unique_effect.max_considered_stacks) {
                        return;
                    } else {
                        ++conversion_unique_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                unique_effect_conversion_ptr->unique_effect.unique_effect_key,
                                owner_actor,
                                other_actor)];
                    }
                }
                if (effect_conversion_ptr) {
                    if (conversion_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                effect_conversion_ptr->effect, owner_actor, other_actor)] >=
                        utils::get_max_considered_stacks_of_effect_type(
                            effect_conversion_ptr->effect)) {
                        return;
                    } else {
                        ++conversion_effect_and_owner_actor_and_other_actor_to_occurrences_map
                            [std::make_tuple(
                                effect_conversion_ptr->effect, owner_actor, other_actor)];
                    }
                }
                for (auto& attribute_conversion : is_attribute_conversion.attribute_conversions) {
                    if (utils::independent_conditions_satisfied(
                            attribute_conversion.condition, owner_actor, other_actor, registry)) {
                        owner_actor_to_attribute_conversion_bonuses[std::make_tuple(
                            owner_actor, attribute_conversion.to)] +=
                            utils::round_to_nearest_even(
                                (relative_attributes.get(other_actor, attribute_conversion.from) *
                                 attribute_conversion.multiplier) +
                                attribute_conversion.addend);
                    }
                }
            });

        for (auto&& [owner_actor_attribute_tuple, bonus] :
             owner_actor_to_attribute_conversion_bonuses) {
            auto& [owner_actor, attribute] = owner_actor_attribute_tuple;
            auto& relative_attributes = registry.get<component::relative_attributes>(owner_actor);
            relative_attributes.set(
                other_actor, attribute, relative_attributes.get(other_actor, attribute) + bonus);
            // spdlog::info("{}:{} conversion {} bonus {} final {}",
            //              utils::get_entity_name(owner_actor, registry),
            //              utils::get_entity_name(other_actor, registry),
            //              utils::to_string(attribute),
            //              bonus,
            //              utils::to_string(relative_attributes.get(other_actor, attribute)));
        }
    });

    registry.view<component::relative_attributes>().each(
        [&](entity_t, component::relative_attributes& relative_attributes) {
            registry.view<component::relative_attributes>().each(
                [&](entity_t other_actor, const component::relative_attributes&) {
                    double precision =
                        relative_attributes.get(other_actor, actor::attribute_t::PRECISION);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::CRITICAL_CHANCE_MULTIPLIER) +
                            (precision - 895) / 2100.0);

                    double ferocity =
                        relative_attributes.get(other_actor, actor::attribute_t::FEROCITY);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::CRITICAL_DAMAGE_MULTIPLIER) +
                            ferocity / 1500.0);

                    double expertise =
                        relative_attributes.get(other_actor, actor::attribute_t::EXPERTISE);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::CONDITION_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::CONDITION_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::BLEEDING_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::BLEEDING_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::BURNING_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::BURNING_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::CONFUSION_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::CONFUSION_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::POISON_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::POISON_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                    relative_attributes.set(
                        other_actor,
                        actor::attribute_t::TORMENT_DURATION_MULTIPLIER,
                        relative_attributes.get(other_actor,
                                                actor::attribute_t::TORMENT_DURATION_MULTIPLIER) +
                            expertise / 1500.0);
                });
        });
}

}  // namespace gw2combat::system
