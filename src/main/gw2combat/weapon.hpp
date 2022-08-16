#ifndef GW2COMBAT_WEAPON_HPP
#define GW2COMBAT_WEAPON_HPP

#include <cstdint>

namespace gw2combat {

enum class weapon_set : std::uint8_t
{
    SET_1,
    SET_2,
};

enum class weapon_sigil : std::uint8_t
{
    AIR,
    BURSTING,
    EARTH,
    TORMENT,
    FORCE,
    GEOMANCY,
    IMPACT,
};

enum class weapon_position : std::uint8_t
{
    MAIN_HAND,
    OFF_HAND,
    TWO_HANDED,
};

enum class weapon_type : std::uint32_t
{
    UNDEFINED,
    EMPTY_HANDED,
    GREATSWORD,
    SWORD,
    AXE,
    TORCH,
    SCEPTER,
    FOCUS,
    KIT_CONJURE,
    TOME,
};

struct weapon_configuration {
    weapon_type type;
    weapon_position position;
    weapon_sigil sigil;
    weapon_set set;
};

static inline std::unordered_map<weapon_type, std::array<double, 2>>
    weapon_type_to_strength_range_map{
        {weapon_type::UNDEFINED, {0.0, 0.0}},
        {weapon_type::EMPTY_HANDED, {656.0, 725.0}},
        {weapon_type::GREATSWORD, {1045.0, 1155.0}},
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
                                 {weapon_set::SET_1, "SET_1"},
                                 {weapon_set::SET_2, "SET_2"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_sigil,
                             {
                                 {weapon_sigil::AIR, "AIR"},
                                 {weapon_sigil::BURSTING, "BURSTING"},
                                 {weapon_sigil::EARTH, "EARTH"},
                                 {weapon_sigil::TORMENT, "TORMENT"},
                                 {weapon_sigil::FORCE, "FORCE"},
                                 {weapon_sigil::GEOMANCY, "GEOMANCY"},
                                 {weapon_sigil::IMPACT, "IMPACT"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_position,
                             {
                                 {weapon_position::MAIN_HAND, "MAIN_HAND"},
                                 {weapon_position::OFF_HAND, "OFF_HAND"},
                                 {weapon_position::TWO_HANDED, "TWO_HANDED"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_type,
                             {
                                 {weapon_type::UNDEFINED, "UNDEFINED"},
                                 {weapon_type::EMPTY_HANDED, "EMPTY_HANDED"},
                                 {weapon_type::GREATSWORD, "GREATSWORD"},
                                 {weapon_type::SWORD, "SWORD"},
                                 {weapon_type::AXE, "AXE"},
                                 {weapon_type::TORCH, "TORCH"},
                                 {weapon_type::SCEPTER, "SCEPTER"},
                                 {weapon_type::FOCUS, "FOCUS"},
                                 {weapon_type::KIT_CONJURE, "KIT_CONJURE"},
                                 {weapon_type::TOME, "TOME"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(weapon_configuration, type, position, sigil, set)

}  // namespace gw2combat

#endif  // GW2COMBAT_WEAPON_HPP
