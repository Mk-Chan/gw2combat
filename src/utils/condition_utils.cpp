#ifndef GW2COMBAT_UTILS_CONDITION_UTILS_HPP
#define GW2COMBAT_UTILS_CONDITION_UTILS_HPP

#include "common.hpp"

#include "basic_utils.hpp"
#include "entity_utils.hpp"
#include "skill_utils.hpp"

#include "configuration/condition.hpp"

#include "component/actor/combat_stats.hpp"
#include "component/actor/relative_attributes.hpp"
#include "component/counter/is_counter.hpp"
#include "component/effect/is_effect.hpp"
#include "component/effect/is_unique_effect.hpp"
#include "component/effect/source_actor.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"
#include "component/temporal/cooldown_component.hpp"

namespace gw2combat::utils {

[[nodiscard]] bool stage_independent_conditions_satisfied(
    const configuration::condition_t& condition,
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
        bool is_satisfied = false;

        auto unique_effect_owners =
            registry.view<component::is_unique_effect, component::owner_component>().each();
        for (auto&& [effect_entity, is_unique_effect, owner_component] : unique_effect_owners) {
            if (is_unique_effect.unique_effect.unique_effect_key ==
                    *condition.unique_effect_on_source &&
                owner_component.entity == source_entity) {
                is_satisfied = true;
                break;
            }
        }

        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.effect_on_source) {
        bool is_satisfied = false;

        auto effect_owners =
            registry.view<component::is_effect, component::owner_component>().each();
        for (auto&& [effect_entity, is_effect, owner_component] : effect_owners) {
            if (is_effect.effect == *condition.effect_on_source &&
                owner_component.entity == source_entity) {
                is_satisfied = true;
                break;
            }
        }

        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.unique_effect_on_target) {
        if (!target_entity) {
            throw std::runtime_error("target_entity must be provided for unique_effect_on_target");
        }

        bool is_satisfied = false;

        auto unique_effect_owners =
            registry.view<component::is_unique_effect, component::owner_component>().each();
        for (auto&& [effect_entity, is_unique_effect, owner_component] : unique_effect_owners) {
            if (is_unique_effect.unique_effect.unique_effect_key ==
                    *condition.unique_effect_on_target &&
                owner_component.entity == target_entity) {
                is_satisfied = true;
                break;
            }
        }

        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.unique_effect_on_target_by_source) {
        if (!target_entity) {
            throw std::runtime_error(
                "target_entity must be provided for unique_effect_on_target_by_source");
        }

        bool is_satisfied = false;

        auto unique_effect_owners = registry
                                        .view<component::is_unique_effect,
                                              component::owner_component,
                                              component::source_actor>()
                                        .each();
        for (auto&& [effect_entity, is_unique_effect, owner_component, source_actor] :
             unique_effect_owners) {
            if (is_unique_effect.unique_effect.unique_effect_key ==
                    *condition.unique_effect_on_target_by_source &&
                owner_component.entity == target_entity && source_actor.entity == source_entity) {
                is_satisfied = true;
                break;
            }
        }

        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.effect_on_target) {
        if (!target_entity) {
            throw std::runtime_error("target_entity must be provided for effect_on_target");
        }

        bool is_satisfied = false;

        auto effect_owners =
            registry.view<component::is_effect, component::owner_component>().each();
        for (auto&& [effect_entity, is_effect, owner_component] : effect_owners) {
            if (is_effect.effect == *condition.effect_on_target &&
                owner_component.entity == *target_entity) {
                is_satisfied = true;
                break;
            }
        }

        if (!is_satisfied) {
            return false;
        }
    }
    if (condition.depends_on_skill_off_cooldown) {
        auto skill_entity = utils::get_skill_entity(
            *condition.depends_on_skill_off_cooldown, source_entity, registry);
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
            double max_health = registry.get<component::relative_attributes>(source_entity)
                                    .get(entity, actor::attribute_t::MAX_HEALTH);
            double current_health = registry.get<component::combat_stats>(source_entity).health;
            double current_health_pct = current_health / max_health;
            if (!threshold_satisfied(current_health_pct)) {
                return false;
            }
        }
        if (condition.threshold->counter_value_subject_to_threshold) {
            bool found = false;
            int counter_value;
            for (auto&& [counter_entity, is_counter] :
                 registry.view<component::is_counter>().each()) {
                if (is_counter.counter_configuration.counter_key ==
                    *condition.threshold->counter_value_subject_to_threshold) {
                    found = true;
                    counter_value = is_counter.value;
                }
            }
            if (!found) {
                throw std::runtime_error(
                    fmt::format("counter with name {} not found",
                                *condition.threshold->counter_value_subject_to_threshold));
            }
            if (!threshold_satisfied(counter_value)) {
                return false;
            }
        }
    }
    if (!condition.not_conditions.empty()) {
        bool not_conditions_satisfied =
            std::any_of(condition.not_conditions.begin(),
                        condition.not_conditions.end(),
                        [&](auto&& condition) {
                            return !stage_independent_conditions_satisfied(
                                condition, entity, target_entity, registry);
                        });
        if (!not_conditions_satisfied) {
            return false;
        }
    }
    if (!condition.or_conditions.empty()) {
        bool or_conditions_satisfied = std::any_of(
            condition.or_conditions.begin(), condition.or_conditions.end(), [&](auto&& condition) {
                return stage_independent_conditions_satisfied(
                    condition, entity, target_entity, registry);
            });
        if (!or_conditions_satisfied) {
            return false;
        }
    }
    if (!condition.and_conditions.empty()) {
        bool and_conditions_satisfied =
            std::all_of(condition.and_conditions.begin(),
                        condition.and_conditions.end(),
                        [&](auto&& condition) {
                            return stage_independent_conditions_satisfied(
                                condition, entity, target_entity, registry);
                        });
        if (!and_conditions_satisfied) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] bool independent_conditions_satisfied(const configuration::condition_t& condition,
                                                    entity_t entity,
                                                    std::optional<entity_t> target_entity,
                                                    registry_t& registry) {
    return (!condition.only_applies_on_strikes || !*condition.only_applies_on_strikes) &&
           (!condition.only_applies_on_finished_casting ||
            !*condition.only_applies_on_finished_casting) &&
           stage_independent_conditions_satisfied(condition, entity, target_entity, registry);
}

[[nodiscard]] bool on_finished_casting_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry) {
    return condition.only_applies_on_finished_casting &&
           *condition.only_applies_on_finished_casting &&
           (!condition.only_applies_on_finished_casting_skill ||
            *condition.only_applies_on_finished_casting_skill ==
                source_skill_configuration.skill_key) &&
           (!condition.only_applies_on_finished_casting_skill_with_tag ||
            utils::skill_has_tag(source_skill_configuration,
                                 *condition.only_applies_on_finished_casting_skill_with_tag)) &&
           stage_independent_conditions_satisfied(condition, entity, std::nullopt, registry);
}

[[nodiscard]] bool on_strike_conditions_satisfied(
    const configuration::condition_t& condition,
    entity_t entity,
    entity_t target_entity,
    bool is_critical,
    const configuration::skill_t& source_skill_configuration,
    registry_t& registry) {
    return condition.only_applies_on_strikes && *condition.only_applies_on_strikes &&
           (!condition.only_applies_on_critical_strikes ||
            (*condition.only_applies_on_critical_strikes && is_critical)) &&
           (!condition.only_applies_on_strikes_by_skill ||
            *condition.only_applies_on_strikes_by_skill == source_skill_configuration.skill_key) &&
           (!condition.only_applies_on_strikes_by_skill_with_tag ||
            utils::skill_has_tag(source_skill_configuration,
                                 *condition.only_applies_on_strikes_by_skill_with_tag)) &&
           stage_independent_conditions_satisfied(condition, entity, target_entity, registry);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_CONDITION_UTILS_HPP
