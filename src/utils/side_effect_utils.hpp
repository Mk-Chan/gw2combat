#ifndef GW2COMBAT_UTILS_SIDE_EFFECT_UTILS_HPP
#define GW2COMBAT_UTILS_SIDE_EFFECT_UTILS_HPP

#include "common.hpp"

#include "actor_utils.hpp"
#include "counter_utils.hpp"
#include "entity_utils.hpp"

#include "component/actor/is_cooldown_modifier.hpp"
#include "component/counter/is_counter_modifier.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_effect_removal.hpp"
#include "component/effect/is_skill_trigger.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/lifecycle/destroy_entity.hpp"

namespace gw2combat::utils {

template <typename T>
inline void apply_side_effects(registry_t& registry,
                               entity_t source_entity,
                               T side_effect_condition_fn) {
    auto source_entity_owner = utils::get_owner(source_entity, registry);
    registry.view<component::is_counter_modifier_t>().each(
        [&](entity_t counter_modifier_entity,
            const component::is_counter_modifier_t& is_counter_modifier) {
            auto owner_actor = utils::get_owner(counter_modifier_entity, registry);
            if (owner_actor != source_entity_owner) {
                return;
            }
            for (auto& counter_modifier : is_counter_modifier.counter_modifiers) {
                auto& counter = utils::get_counter(counter_modifier.counter_key, registry);
                if (side_effect_condition_fn(counter_modifier.condition)) {
                    utils::apply_counter_modifications(registry, counter, counter_modifier);
                }
            }
        });
    registry.view<component::is_cooldown_modifier_t>().each(
        [&](entity_t cooldown_modifier_entity,
            const component::is_cooldown_modifier_t& is_cooldown_modifier) {
            auto owner_actor = utils::get_owner(cooldown_modifier_entity, registry);
            if (owner_actor != source_entity_owner) {
                return;
            }
            for (auto& cooldown_modifier : is_cooldown_modifier.cooldown_modifiers) {
                if (side_effect_condition_fn(cooldown_modifier.condition)) {
                    utils::apply_cooldown_modifications(registry, owner_actor, cooldown_modifier);
                }
            }
        });
    registry.view<component::is_effect_removal>().each(
        [&](entity_t effect_removal_entity, component::is_effect_removal& is_effect_removal) {
            auto owner_entity = utils::get_owner(effect_removal_entity, registry);
            if (owner_entity != source_entity_owner) {
                return;
            }

            auto& effect_removal = is_effect_removal.effect_removal;
            if (side_effect_condition_fn(effect_removal.condition)) {
                if (effect_removal.effect != actor::effect_t::INVALID) {
                    int stacks_to_remove =
                        effect_removal.num_stacks ? *effect_removal.num_stacks : 5000;
                    registry.view<component::is_effect, component::owner_component>().each(
                        [&](entity_t effect_entity,
                            const component::is_effect& is_effect,
                            const component::owner_component& effect_owner) {
                            if (effect_owner.entity == source_entity_owner &&
                                is_effect.effect == effect_removal.effect) {
                                if (stacks_to_remove <= 0) {
                                    return;
                                }
                                --stacks_to_remove;
                                registry.emplace_or_replace<component::destroy_entity>(
                                    effect_entity);
                            }
                        });
                }
                if (!effect_removal.unique_effect.empty()) {
                    int stacks_to_remove =
                        effect_removal.num_stacks ? *effect_removal.num_stacks : 5000;
                    registry.view<component::is_unique_effect, component::owner_component>().each(
                        [&](entity_t unique_effect_entity,
                            const component::is_unique_effect& is_unique_effect,
                            const component::owner_component& effect_owner) {
                            if (effect_owner.entity == source_entity_owner &&
                                is_unique_effect.unique_effect.unique_effect_key ==
                                    effect_removal.unique_effect) {
                                if (stacks_to_remove <= 0) {
                                    return;
                                }
                                --stacks_to_remove;
                                registry.emplace_or_replace<component::destroy_entity>(
                                    unique_effect_entity);
                            }
                        });
                }
            }
        });
    registry.view<component::is_skill_trigger>().each(
        [&](entity_t skill_trigger_entity, component::is_skill_trigger& is_skill_trigger) {
            auto owner_entity = utils::get_owner(skill_trigger_entity, registry);
            if (owner_entity != source_entity_owner) {
                return;
            }

            if (!is_skill_trigger.already_triggered &&
                side_effect_condition_fn(is_skill_trigger.skill_trigger.condition)) {
                is_skill_trigger.already_triggered = true;
                utils::enqueue_child_skill(
                    is_skill_trigger.skill_trigger.skill_key, source_entity_owner, registry);
            }
        });
    registry.view<component::is_unchained_skill_trigger>().each(
        [&](entity_t skill_trigger_entity,
            const component::is_unchained_skill_trigger& is_unchained_skill_trigger) {
            auto owner_entity = utils::get_owner(skill_trigger_entity, registry);
            if (owner_entity != source_entity_owner) {
                return;
            }
            auto& skill_trigger = is_unchained_skill_trigger.skill_trigger;
            if (side_effect_condition_fn(skill_trigger.condition)) {
                utils::enqueue_child_skill(skill_trigger.skill_key, source_entity_owner, registry);
            }
        });
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_SIDE_EFFECT_UTILS_HPP
