#ifndef GW2COMBAT_UTILS_GEAR_UTILS_HPP
#define GW2COMBAT_UTILS_GEAR_UTILS_HPP

#include "common.hpp"

#include "actor/weapon.hpp"

#include "component/equipment/weapons.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline double get_weapon_strength(entity_t actor_entity,
                                                       actor::weapon_type type,
                                                       registry_t& registry,
                                                       bool random = false) {
    if (type == actor::weapon_type::INVALID) {
        throw std::runtime_error("invalid weapon_type");
    }
    auto effective_weapon_type = [&]() {
        if (type == actor::weapon_type::MAIN_HAND) {
            auto& equipped_weapons = registry.get<component::equipped_weapons>(actor_entity);
            auto& current_weapon_set = registry.get<component::current_weapon_set>(actor_entity);
            auto currently_equipped_main_hand_weapon =
                std::find_if(equipped_weapons.weapons.begin(),
                             equipped_weapons.weapons.end(),
                             [&](const component::weapon_t& weapon) {
                                 return weapon.set == current_weapon_set.set &&
                                        (weapon.position == actor::weapon_position::MAIN_HAND ||
                                         weapon.position == actor::weapon_position::TWO_HANDED);
                             });
            return currently_equipped_main_hand_weapon->type;
        } else {
            return type;
        }
    }();
    auto& range = actor::weapon_type_to_strength_range_map.at(effective_weapon_type);
    return random ? utils::get_random(range[0], range[1]) : (range[0] + range[1]) / 2.0;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_GEAR_UTILS_HPP
