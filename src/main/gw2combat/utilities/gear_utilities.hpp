#ifndef GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/component/gear/weapon_configurations.hpp"
#include "gw2combat/weapon.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool has_weapon_type(weapon_type type,
                                                 weapon_position position,
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
                           return configuration.set == current_set && configuration.type == type &&
                                  configuration.position == position;
                       });
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

[[nodiscard]] static inline bool has_one_handed_weapon(weapon_position position,
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
                           return configuration.set == current_set &&
                                  is_one_handed_weapon(configuration.type) &&
                                  configuration.position == position;
                       });
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

[[nodiscard]] static inline bool has_sigil_at_all(weapon_sigil sigil,
                                                  entity_t entity,
                                                  registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return ranges::find_if(weapon_configurations, [&](const weapon_configuration& configuration) {
               return configuration.sigil == sigil;
           }) != ranges::end(weapon_configurations);
}

[[nodiscard]] static inline bool has_sigil_equipped(weapon_sigil sigil,
                                                    entity_t entity,
                                                    registry_t& registry) {
    if (!registry.any_of<component::equipped_weapon_set>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::equipped_weapon_set>(entity).current_set;
    auto& weapon_configurations =
        registry.get<component::available_weapon_configurations>(entity).weapon_configurations;
    return ranges::find_if(weapon_configurations, [&](const weapon_configuration& configuration) {
               return configuration.set == current_set && configuration.sigil == sigil;
           }) != ranges::end(weapon_configurations);
}

[[nodiscard]] static inline double get_weapon_strength(weapon_type type) {
    auto& range = weapon_type_to_strength_range_map.at(type);
    return range[0] + (get_random_0_100() * (range[1] - range[0]) / 100.0);
    // return (range[0] + range[1]) / 2.0;
}

[[nodiscard]] static inline bool has_rune(rune_type rune, entity_t entity, registry_t& registry) {
    return registry.any_of<component::rune_component>(entity) &&
           registry.get<component::rune_component>(entity).rune == rune;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP
