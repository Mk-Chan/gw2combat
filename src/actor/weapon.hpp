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
    DAGGER,
    MACE,
    PISTOL,
    SHIELD,
    WARHORN,
    HAMMER,
    RIFLE,
    SHORTBOW,
    STAFF,
    AQUATIC
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
        {weapon_type::DAGGER, {970.0, 1030.0}},
        {weapon_type::MACE, {940.0, 1060.0}},
        {weapon_type::PISTOL, {920.0, 1080.0}},
        {weapon_type::SHIELD, {846.0, 954.0}},
        {weapon_type::WARHORN, {855.0, 945.0}},
        {weapon_type::HAMMER, {1034.0, 1166.0}},
        {weapon_type::RIFLE, {1035.0, 1265.0}},
        {weapon_type::SHORTBOW, {950.0, 1050.0}},
        {weapon_type::STAFF, {1034.0, 1166.0}},
        {weapon_type::AQUATIC, {950.0, 1050.0}},
    };

NLOHMANN_JSON_SERIALIZE_ENUM(weapon_set,
                             {
                                 {weapon_set::INVALID, "invalid"},

                                 {weapon_set::SET_1, "set_1"},
                                 {weapon_set::SET_2, "set_2"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_position,
                             {
                                 {weapon_position::INVALID, "invalid"},

                                 {weapon_position::UNIVERSAL, "universal"},
                                 {weapon_position::MAIN_HAND, "main_hand"},
                                 {weapon_position::OFF_HAND, "off_hand"},
                                 {weapon_position::TWO_HANDED, "two_handed"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_type,
                             {
                                 {weapon_type::INVALID, "invalid"},

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
                                 {weapon_type::DAGGER, "dagger"},
                                 {weapon_type::MACE, "mace"},
                                 {weapon_type::PISTOL, "pistol"},
                                 {weapon_type::SHIELD, "shield"},
                                 {weapon_type::WARHORN, "warhorn"},
                                 {weapon_type::HAMMER, "hammer"},
                                 {weapon_type::RIFLE, "rifle"},
                                 {weapon_type::SHORTBOW, "shortbow"},
                                 {weapon_type::STAFF, "staff"},
                                 {weapon_type::AQUATIC, "aquatic"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_WEAPON_HPP
