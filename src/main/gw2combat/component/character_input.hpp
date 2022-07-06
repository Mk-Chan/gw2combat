#ifndef GW2COMBAT_COMPONENT_CHARACTER_INPUT_HPP
#define GW2COMBAT_COMPONENT_CHARACTER_INPUT_HPP

#include <cstdint>

#include "gw2combat/types.hpp"

namespace gw2combat::component {

struct character_input {
    enum class command : std::uint32_t
    {
        NOTHING = 0x0,

        WEAPON_SKILL_1 = 0x1,
        WEAPON_SKILL_2 = 0x2,
        WEAPON_SKILL_3 = 0x4,
        WEAPON_SKILL_4 = 0x8,
        WEAPON_SKILL_5 = 0x10,
        WEAPON_SWAP = 0x20,

        UTILITY_SKILL_1 = 0x40,
        UTILITY_SKILL_2 = 0x80,
        UTILITY_SKILL_3 = 0x100,
        UTILITY_SKILL_4 = 0x200,
        UTILITY_SKILL_5 = 0x400,

        F_SKILL_1 = 0x1000,
        F_SKILL_2 = 0x2000,
        F_SKILL_3 = 0x4000,
        F_SKILL_4 = 0x8000,
        F_SKILL_5 = 0x10000,

        HEAL_SKILL = UTILITY_SKILL_1,
        ELITE_SKILL = UTILITY_SKILL_5,
    };

    [[nodiscard]] inline bool contains(std::initializer_list<command> commands) const {
        auto bitset = static_cast<std::underlying_type<command>::type>(input_bitset);
        auto mask = static_cast<std::underlying_type<command>::type>(0);
        std::for_each(commands.begin(), commands.end(), [&](const auto& item) {
            mask |= static_cast<std::underlying_type<command>::type>(item);
        });
        return static_cast<bool>(bitset & mask);
    }

    static inline command make_command(std::initializer_list<command> commands) {
        auto result = static_cast<std::underlying_type<command>::type>(0);
        std::for_each(commands.begin(), commands.end(), [&](const auto& item) {
            result |= static_cast<std::underlying_type<command>::type>(item);
        });
        return command{result};
    }

    command input_bitset;
    tick_t received_at;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_CHARACTER_INPUT_HPP
