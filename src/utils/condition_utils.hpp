#ifndef GW2COMBAT_UTILS_CONDITION_UTILS_HPP
#define GW2COMBAT_UTILS_CONDITION_UTILS_HPP

#include "common.hpp"

#include "component/actor/effects_component.hpp"
#include "component/actor/unique_effects_component.hpp"
#include "component/equipment/bundle.hpp"
#include "component/equipment/weapons.hpp"

#include "configuration/condition.hpp"

namespace gw2combat::utils {

bool conditions_satisfied(const configuration::condition_t& condition,
                          registry_t& registry,
                          entity_t source_entity,
                          std::optional<entity_t> target_entity = std::nullopt) {
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
            throw std::runtime_error(
                "target_entity is a required parameter for target-based conditions");
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
    if (condition.effect_on_target) {
        if (!target_entity) {
            throw std::runtime_error(
                "target_entity is a required parameter for target-based conditions");
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
    // Put a component on the source about which skills have been cast and which targets they will
    // hit instead of the strike_pipeline. Then iterate over that to check if skill is cast by
    // source
    return true;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_CONDITION_UTILS_HPP
