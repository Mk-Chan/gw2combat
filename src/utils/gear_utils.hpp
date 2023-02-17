#ifndef GW2COMBAT_UTILS_GEAR_UTILS_HPP
#define GW2COMBAT_UTILS_GEAR_UTILS_HPP

#include "common.hpp"

#include "actor/weapon.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline double get_weapon_strength(actor::weapon_type type) {
    if (type == actor::weapon_type::INVALID) {
        throw std::runtime_error("invalid weapon_type");
    }
    auto& range = actor::weapon_type_to_strength_range_map.at(type);
    return (range[0] + range[1]) / 2.0;
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_GEAR_UTILS_HPP
