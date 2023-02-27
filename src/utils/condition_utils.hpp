#ifndef GW2COMBAT_UTILS_CONDITION_UTILS_HPP
#define GW2COMBAT_UTILS_CONDITION_UTILS_HPP

#include "common.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/counters_component.hpp"
#include "component/actor/effects_component.hpp"
#include "component/actor/skills_component.hpp"
#include "component/actor/unique_effects_component.hpp"
#include "component/counter/is_counter.hpp"
#include "component/effect/source_actor.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/temporal/cooldown_component.hpp"

#include "configuration/condition.hpp"

#include "basic_utils.hpp"
#include "entity_utils.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool conditions_satisfied(const configuration::condition_t& condition,
                                                      entity_t entity,
                                                      std::optional<entity_t> target_entity,
                                                      registry_t& registry) {
    auto source_entity = utils::get_owner(entity, registry);
    if (condition.weapon_type || condition.weapon_position) {
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
            [&](const component::weapon_t& weapon) {
                return weapon.set == current_weapon_set.set &&
                       (!condition.weapon_type ||
                        (!has_bundle_equipped && weapon.type == *condition.weapon_type)) &&
                       (!condition.weapon_position ||
                        (!has_bundle_equipped && weapon.position == *condition.weapon_position));
            });
        if (!is_satisfied) {
            return false;
        }
    }

    if (condition.weapon_set) {
        if (!registry.any_of<component::current_weapon_set>(source_entity)) {
            return false;
        }
        auto& current_weapon_set = registry.get<component::current_weapon_set>(source_entity);
        bool is_satisfied = current_weapon_set.set == *condition.weapon_set;
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.unique_effect_on_source) {
        if (!registry.any_of<component::unique_effects_component>(source_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::unique_effects_component>(source_entity)
                                .has(*condition.unique_effect_on_source);
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.effect_on_source) {
        if (!registry.any_of<component::effects_component>(source_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::effects_component>(source_entity)
                                .has(*condition.effect_on_source);
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.unique_effect_on_target) {
        if (!target_entity) {
            throw std::runtime_error("target_entity must be provided for unique_effect_on_target");
        }
        if (!registry.any_of<component::unique_effects_component>(*target_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::unique_effects_component>(*target_entity)
                                .has(*condition.unique_effect_on_target);
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.unique_effect_on_target_by_source) {
        if (!target_entity) {
            throw std::runtime_error(
                "target_entity must be provided for unique_effect_on_target_by_source");
        }
        if (!registry.any_of<component::unique_effects_component>(*target_entity)) {
            return false;
        }
        auto unique_effect_entities =
            registry.get<component::unique_effects_component>(*target_entity)
                .find_by(*condition.unique_effect_on_target_by_source);
        bool is_satisfied =
            std::any_of(unique_effect_entities.begin(),
                        unique_effect_entities.end(),
                        [&](entity_t unique_effect_entity) {
                            auto source_actor_ptr =
                                registry.try_get<component::source_actor>(unique_effect_entity);
                            return source_actor_ptr && source_actor_ptr->entity == source_entity;
                        });
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.effect_on_target) {
        if (!target_entity) {
            throw std::runtime_error("target_entity must be provided for effect_on_target");
        }
        if (!registry.any_of<component::effects_component>(*target_entity)) {
            return false;
        }
        bool is_satisfied = registry.get<component::effects_component>(*target_entity)
                                .has(*condition.effect_on_target);
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.depends_on_skill_off_cooldown) {
        auto& skills_component = registry.get<component::skills_component>(source_entity);
        auto skill_entity = skills_component.find_by(*condition.depends_on_skill_off_cooldown);
        bool is_satisfied = !registry.any_of<component::cooldown_component>(skill_entity);
        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.threshold) {
        auto threshold_satisfied = [&](double number_subject_to_threshold) {
            switch (condition.threshold->threshold_type) {
                case configuration::threshold_t::type::EQUAL:
                    return number_subject_to_threshold == condition.threshold->threshold_value;
                case configuration::threshold_t::type::UPPER_BOUND_EXCLUSIVE:
                    return number_subject_to_threshold < condition.threshold->threshold_value;
                case configuration::threshold_t::type::UPPER_BOUND_INCLUSIVE:
                    return number_subject_to_threshold <= condition.threshold->threshold_value;
                case configuration::threshold_t::type::LOWER_BOUND_EXCLUSIVE:
                    return number_subject_to_threshold > condition.threshold->threshold_value;
                case configuration::threshold_t::type::LOWER_BOUND_INCLUSIVE:
                    return number_subject_to_threshold >= condition.threshold->threshold_value;
                default:
                    throw std::runtime_error("threshold_type not implemented yet");
            }
        };
        if (condition.threshold->threshold_type == configuration::threshold_t::type::INVALID) {
            throw std::runtime_error("invalid threshold_type");
        }
        if (condition.threshold->generate_random_number_subject_to_threshold &&
            *condition.threshold->generate_random_number_subject_to_threshold) {
            if (!threshold_satisfied(utils::get_random_0_100())) {
                return false;
            }
        }
        if (condition.threshold->health_pct_subject_to_threshold &&
            *condition.threshold->health_pct_subject_to_threshold) {
            double max_health = registry.get<component::relative_attributes>(entity).get(
                entity, actor::attribute_t::MAX_HEALTH);
            double current_health = registry.get<component::combat_stats>(entity).health;
            double current_health_pct = current_health / max_health;
            if (!threshold_satisfied(current_health_pct)) {
                return false;
            }
        }
        if (condition.threshold->counter_value_subject_to_threshold) {
            auto counter_entity = registry.get<component::counters_component>(entity).find_by(
                *condition.threshold->counter_value_subject_to_threshold);
            if (!counter_entity) {
                throw std::runtime_error(
                    fmt::format("counter with name {} not found",
                                *condition.threshold->counter_value_subject_to_threshold));
            }
            int counter_value = registry.get<component::is_counter>(*counter_entity).value;
            if (!threshold_satisfied(counter_value)) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_CONDITION_UTILS_HPP
