#ifndef GW2COMBAT_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_HPP

#include <algorithm>

#include "types.hpp"

#include "effects.hpp"

#include "gw2combat/component/damage/source_entity.hpp"
#include "gw2combat/component/gear/runes.hpp"
#include "gw2combat/component/gear/weapon_configurations.hpp"
#include "gw2combat/component/traits.hpp"
#include "gw2combat/skills.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool skill_has_tag(const skills::skill& skill,
                                               skills::skill_tag skill_tag) {
    return std::find(skill.tags.begin(), skill.tags.end(), skill_tag) != skill.tags.end();
}

[[nodiscard]] static inline bool has_trait(trait_type trait,
                                           entity_t entity,
                                           registry_t& registry) {
    if (!registry.any_of<component::traits_component>(entity)) {
        return false;
    }

    auto& traits_component = registry.get<component::traits_component>(entity);
    return std::find(traits_component.traits.begin(), traits_component.traits.end(), trait) !=
           traits_component.traits.end();
}

[[nodiscard]] static inline bool has_rune(rune_type rune, entity_t entity, registry_t& registry) {
    return registry.any_of<component::rune_component>(entity) &&
           registry.get<component::rune_component>(entity).rune == rune;
}

[[nodiscard]] static inline bool has_weapon_type(weapon_type type,
                                                 entity_t entity,
                                                 registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return std::any_of(weapon_configurations.begin(),
                       weapon_configurations.end(),
                       [&](const weapon_configuration& configuration) {
                           return configuration.set == current_set && configuration.type == type;
                       });
}

[[nodiscard]] static inline bool is_one_handed_weapon(weapon_type type) {
    switch (type) {
        case weapon_type::EMPTY_HANDED:
        case weapon_type::GREATSWORD:
        case weapon_type::KIT_CONJURE_TOME:
            return false;
        case weapon_type::SWORD:
        case weapon_type::AXE:
        case weapon_type::TORCH:
        case weapon_type::FOCUS:
        case weapon_type::SCEPTER:
            return true;
        default:
            throw std::runtime_error("Unknown weapon type");
    }
}

[[nodiscard]] static inline bool is_two_handed_weapon(weapon_type type) {
    return !is_one_handed_weapon(type);
}

[[nodiscard]] static inline bool has_one_handed_weapon(entity_t entity, registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return std::any_of(weapon_configurations.begin(),
                       weapon_configurations.end(),
                       [&](const weapon_configuration& configuration) {
                           return configuration.set == current_set &&
                                  is_one_handed_weapon(configuration.type);
                       });
}

[[nodiscard]] static inline bool has_two_handed_weapon(entity_t entity, registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return std::any_of(weapon_configurations.begin(),
                       weapon_configurations.end(),
                       [&](const weapon_configuration& configuration) {
                           return configuration.set == current_set &&
                                  is_two_handed_weapon(configuration.type);
                       });
}

[[nodiscard]] static inline bool has_sigil(weapon_sigil sigil,
                                           entity_t entity,
                                           registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return std::find_if(weapon_configurations.begin(),
                        weapon_configurations.end(),
                        [&](const weapon_configuration& configuration) {
                            return configuration.set == current_set && configuration.sigil == sigil;
                        }) != weapon_configurations.end();
}

[[nodiscard]] static inline std::string get_entity_name(entity_t entity, registry_t& registry) {
    if (!registry.ctx().contains<entt::hashed_string>(to_u32(entity))) {
        return "temporary_entity";
    }
    return std::string{registry.ctx().at<entt::hashed_string>(to_u32(entity)).data()};
}

[[nodiscard]] static inline double get_weapon_strength(weapon_type type) {
    auto& range = weapon_type_to_strength_range_map.at(type);
    return (range[0] + range[1]) / 2.0;
}

[[nodiscard]] static inline entity_t get_source_entity(entity_t entity, registry_t& registry) {
    entity_t current_entity = entity;
    while (true) {
        auto outgoing_damage_source_ptr =
            registry.try_get<component::source_entity>(current_entity);
        if (outgoing_damage_source_ptr == nullptr) {
            break;
        }

        current_entity = outgoing_damage_source_ptr->entity;
    }
    return current_entity;
}

[[nodiscard]] static inline double get_critical_hit_multiplier(
    const component::effective_attributes& effective_attributes) {
    return (1.0 + (std::min(effective_attributes.critical_chance_pct, 100.0) / 100.0) *
                      (effective_attributes.critical_damage_pct / 100.0 - 1.0));
}

[[nodiscard]] static inline tick_t get_effective_condition_duration(
    tick_t duration,
    effects::applied_effect_type effect_type,
    const component::effective_attributes& effective_attributes) {
    auto calc = [&](double special_condition_duration_pct) {
        return tick_t{(unsigned int)((double)duration *
                                     (1.0 + std::max(effective_attributes.condition_duration_pct,
                                                     special_condition_duration_pct) /
                                                100.0))};
    };

    switch (effect_type) {
        case effects::applied_effect_type::BURNING:
            return calc(effective_attributes.burning_duration_pct);
        case effects::applied_effect_type::BLEEDING:
            return calc(effective_attributes.bleeding_duration_pct);
        case effects::applied_effect_type::BINDING_BLADE:
            return tick_t{duration};
    }
}

static inline int critical_chance_pct_to_precision(double critical_chance_pct) {
    return (int)(critical_chance_pct * 21.0 + 895.0);
}

static inline double calculate_critical_chance_multiplier(unsigned int precision,
                                                          unsigned int ferocity) {
    double critical_chance = 0.05 + ((double)precision - 1000.0) / 2100.0;
    double critical_damage = 1.5 + (double)ferocity / 1500.0;
    return 1.0 + std::min(critical_chance, 1.0) * (critical_damage - 1.0);
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_HPP
