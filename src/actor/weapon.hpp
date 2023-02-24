#ifndef GW2COMBAT_ACTOR_WEAPON_HPP
#define GW2COMBAT_ACTOR_WEAPON_HPP

#include <unordered_map>

#include "common.hpp"

namespace gw2combat::actor {

enum class weapon_set : std::uint8_t
{
    INVALID,

    SET_1,
    SET_2,
};

enum class weapon_position : std::uint8_t
{
    INVALID,

    UNIVERSAL,
    MAIN_HAND,
    OFF_HAND,
    TWO_HANDED,
};

enum class weapon_type : std::uint32_t
{
    INVALID,

    MAIN_HAND,

    EMPTY_HANDED,
    GREATSWORD,
    LONGBOW,
    SWORD,
    AXE,
    TORCH,
    SCEPTER,
    FOCUS,
    KIT_CONJURE,
    TOME,
};

static inline std::unordered_map<weapon_type, std::array<double, 2>>
    weapon_type_to_strength_range_map{
        {weapon_type::INVALID, {0.0, 0.0}},

        {weapon_type::EMPTY_HANDED, {656.0, 725.0}},
        {weapon_type::GREATSWORD, {1045.0, 1155.0}},
        {weapon_type::LONGBOW, {966.0, 1134.0}},
        {weapon_type::SWORD, {950.0, 1050.0}},
        {weapon_type::AXE, {900.0, 1100.0}},
        {weapon_type::TORCH, {828.0, 972.0}},
        {weapon_type::SCEPTER, {940.0, 1060.0}},
        {weapon_type::FOCUS, {873.0, 927.0}},
        {weapon_type::KIT_CONJURE, {920.0, 1017.0}},
        {weapon_type::TOME, {876.0, 969.0}},
    };

NLOHMANN_JSON_SERIALIZE_ENUM(weapon_set,
                             {
                                 {weapon_set::INVALID, "Invalid"},

                                 {weapon_set::SET_1, "set_1"},
                                 {weapon_set::SET_2, "set_2"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_position,
                             {
                                 {weapon_position::INVALID, "Invalid"},

                                 {weapon_position::UNIVERSAL, "universal"},
                                 {weapon_position::MAIN_HAND, "main_hand"},
                                 {weapon_position::OFF_HAND, "off_hand"},
                                 {weapon_position::TWO_HANDED, "two_handed"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_type,
                             {
                                 {weapon_type::INVALID, "Invalid"},

                                 {weapon_type::MAIN_HAND, "main_hand"},

                                 {weapon_type::EMPTY_HANDED, "empty_handed"},
                                 {weapon_type::GREATSWORD, "greatsword"},
                                 {weapon_type::LONGBOW, "longbow"},
                                 {weapon_type::SWORD, "sword"},
                                 {weapon_type::AXE, "axe"},
                                 {weapon_type::TORCH, "torch"},
                                 {weapon_type::SCEPTER, "scepter"},
                                 {weapon_type::FOCUS, "focus"},
                                 {weapon_type::KIT_CONJURE, "kit_conjure"},
                                 {weapon_type::TOME, "tome"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_WEAPON_HPP
