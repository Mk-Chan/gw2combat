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
    AQUATIC,
    SPEAR
};

static inline std::unordered_map<weapon_type, std::array<int, 2>>
    weapon_type_to_strength_range_map{
        {weapon_type::INVALID, {0, 0}},

        {weapon_type::EMPTY_HANDED, {656, 725}},
        {weapon_type::GREATSWORD, {1045, 1155}},
        {weapon_type::LONGBOW, {966, 1134}},
        {weapon_type::SWORD, {950, 1050}},
        {weapon_type::AXE, {900, 1100}},
        {weapon_type::TORCH, {828, 972}},
        {weapon_type::SCEPTER, {940, 1060}},
        {weapon_type::FOCUS, {873, 927}},
        {weapon_type::KIT_CONJURE, {920, 1017}},
        {weapon_type::TOME, {876, 969}},
        {weapon_type::DAGGER, {970, 1030}},
        {weapon_type::MACE, {940, 1060}},
        {weapon_type::PISTOL, {920, 1080}},
        {weapon_type::SHIELD, {846, 954}},
        {weapon_type::WARHORN, {855, 945}},
        {weapon_type::HAMMER, {1034, 1166}},
        {weapon_type::RIFLE, {1035, 1265}},
        {weapon_type::SHORTBOW, {950, 1050}},
        {weapon_type::STAFF, {1034, 1166}},
        {weapon_type::AQUATIC, {950, 1050}},
        {weapon_type::SPEAR, {950, 1050}},
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
                                 {weapon_type::SPEAR, "spear"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_WEAPON_HPP
