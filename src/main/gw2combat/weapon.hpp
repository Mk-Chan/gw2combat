#ifndef GW2COMBAT_WEAPON_HPP
#define GW2COMBAT_WEAPON_HPP

#include <cstdint>

namespace gw2combat {

enum class weapon_type : std::uint32_t
{
    EMPTY_HANDED,
    GREATSWORD,
    SWORD,
    TORCH,
    FOCUS,
};

static inline std::unordered_map<weapon_type, std::array<int, 2>> weapon_type_to_strength_range_map{
    {weapon_type::EMPTY_HANDED, {656, 725}},
    {weapon_type::GREATSWORD, {1045, 1155}},
    {weapon_type::SWORD, {950, 1050}},
    {weapon_type::TORCH, {828, 972}},
    {weapon_type::FOCUS, {873, 927}}};

static inline double weapon_strength(weapon_type type) {
    auto& range = weapon_type_to_strength_range_map.at(type);
    return (range[0] + range[1]) / 2.0;
}

NLOHMANN_JSON_SERIALIZE_ENUM(weapon_type,
                             {
                                 {weapon_type::EMPTY_HANDED, "EMPTY_HANDED"},
                                 {weapon_type::GREATSWORD, "GREATSWORD"},
                                 {weapon_type::SWORD, "SWORD"},
                                 {weapon_type::TORCH, "TORCH"},
                                 {weapon_type::FOCUS, "FOCUS"},
                             })

}  // namespace gw2combat

#endif  // GW2COMBAT_WEAPON_HPP
