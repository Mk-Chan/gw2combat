#ifndef GW2COMBAT_UTILITIES_ENTITY_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_ENTITY_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/actor/build.hpp"

#include "gw2combat/component/actor/static_attributes.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline std::string get_entity_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<std::string>(entity)) {
        return "temporary_entity";
    }
    return registry.ctx().at<std::string>(entity);
}

[[nodiscard]] static inline component::static_attributes get_static_attributes_from_build(
    const actor::build_t& build) {
    component::static_attributes static_attributes;
    static_attributes.attribute_value_map[actor::attribute_t::POWER] = build.attributes.power;
    static_attributes.attribute_value_map[actor::attribute_t::PRECISION] =
        build.attributes.precision;
    static_attributes.attribute_value_map[actor::attribute_t::TOUGHNESS] =
        build.attributes.toughness;
    static_attributes.attribute_value_map[actor::attribute_t::VITALITY] = build.attributes.vitality;
    static_attributes.attribute_value_map[actor::attribute_t::CONCENTRATION] =
        build.attributes.concentration;
    static_attributes.attribute_value_map[actor::attribute_t::CONDITION_DAMAGE] =
        build.attributes.condition_damage;
    static_attributes.attribute_value_map[actor::attribute_t::EXPERTISE] =
        build.attributes.expertise;
    static_attributes.attribute_value_map[actor::attribute_t::FEROCITY] = build.attributes.ferocity;
    static_attributes.attribute_value_map[actor::attribute_t::HEALING_POWER] =
        build.attributes.healing_power;
    static_attributes.attribute_value_map[actor::attribute_t::ARMOR] = build.attributes.armor;
    static_attributes.attribute_value_map[actor::attribute_t::MAX_HEALTH] =
        build.attributes.max_health;
    static_attributes.attribute_value_map[actor::attribute_t::BOON_DURATION_PCT] =
        build.attributes.boon_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CRITICAL_CHANCE_PCT] =
        build.attributes.critical_chance_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CRITICAL_DAMAGE_PCT] =
        build.attributes.critical_damage_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CONDITION_DURATION_PCT] =
        build.attributes.condition_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::BURNING_DURATION_PCT] =
        build.attributes.burning_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::BLEEDING_DURATION_PCT] =
        build.attributes.bleeding_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::CONFUSION_DURATION_PCT] =
        build.attributes.confusion_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::POISON_DURATION_PCT] =
        build.attributes.poison_duration_pct;
    static_attributes.attribute_value_map[actor::attribute_t::TORMENT_DURATION_PCT] =
        build.attributes.torment_duration_pct;
    return static_attributes;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_ENTITY_UTILITIES_HPP
