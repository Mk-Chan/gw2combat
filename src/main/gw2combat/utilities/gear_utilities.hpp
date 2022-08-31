#ifndef GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/actor/rune.hpp"
#include "gw2combat/actor/weapon.hpp"

#include "gw2combat/component/actor/rune_component.hpp"
#include "gw2combat/component/equipment/weapons.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool is_one_handed_weapon(actor::weapon_type type) {
    switch (type) {
        case actor::weapon_type::EMPTY_HANDED:
        case actor::weapon_type::GREATSWORD:
        case actor::weapon_type::TOME:
        case actor::weapon_type::KIT_CONJURE:
            return false;
        case actor::weapon_type::SWORD:
        case actor::weapon_type::AXE:
        case actor::weapon_type::TORCH:
        case actor::weapon_type::FOCUS:
        case actor::weapon_type::SCEPTER:
            return true;
        default:
            throw std::runtime_error("Unknown weapon type");
    }
}

[[nodiscard]] static inline bool is_two_handed_weapon(actor::weapon_type type) {
    return !is_one_handed_weapon(type);
}

[[nodiscard]] static inline bool has_weapon_type_active(actor::weapon_type type,
                                                        actor::weapon_position position,
                                                        entity_t entity,
                                                        registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.set == current_set && weapon.type == type && weapon.position == position;
    });
}

[[nodiscard]] static inline bool has_weapon_type_active(actor::weapon_type type,
                                                        entity_t entity,
                                                        registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.set == current_set && weapon.type == type;
    });
}

[[nodiscard]] static inline bool has_one_handed_weapon_active(actor::weapon_position position,
                                                              entity_t entity,
                                                              registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.set == current_set && is_one_handed_weapon(weapon.type) &&
               weapon.position == position;
    });
}

[[nodiscard]] static inline bool has_one_handed_weapon_active(entity_t entity,
                                                              registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.set == current_set && is_one_handed_weapon(weapon.type);
    });
}

[[nodiscard]] static inline bool has_two_handed_weapon_active(entity_t entity,
                                                              registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.set == current_set && is_two_handed_weapon(weapon.type);
    });
}

[[nodiscard]] static inline bool has_sigil_at_all(actor::weapon_sigil sigil,
                                                  entity_t entity,
                                                  registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.sigil == sigil;
    });
}

[[nodiscard]] static inline bool has_sigil_equipped(actor::weapon_sigil sigil,
                                                    entity_t entity,
                                                    registry_t& registry) {
    if (!registry.any_of<component::equipped_weapons>(entity)) {
        return false;
    }
    auto& weapons = registry.get<component::equipped_weapons>(entity).weapons;
    auto current_set = registry.get<component::current_weapon_set>(entity).set;
    return std::any_of(weapons.begin(), weapons.end(), [&](const actor::weapon& weapon) {
        return weapon.sigil == sigil && weapon.set == current_set;
    });
}

[[nodiscard]] static inline double get_weapon_strength(actor::weapon_type type) {
    if (type == actor::weapon_type::INVALID) {
        throw std::runtime_error("got invalid weapon_type");
    }
    auto& range = actor::weapon_type_to_strength_range_map.at(type);
    if constexpr (DETERMINISTIC_SIMULATION) {
        return (range[0] + range[1]) / 2.0;
    } else {
        return range[0] + (get_random_0_100() * (range[1] - range[0]) / 100.0);
    }
}

[[nodiscard]] static inline bool has_rune(actor::rune_t rune,
                                          entity_t entity,
                                          registry_t& registry) {
    return registry.any_of<component::rune_component>(entity) &&
           registry.get<component::rune_component>(entity).rune == rune;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_GEAR_UTILITIES_HPP
