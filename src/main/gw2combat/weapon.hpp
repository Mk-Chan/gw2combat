#ifndef GW2COMBAT_WEAPON_HPP
#define GW2COMBAT_WEAPON_HPP

#include <cstdint>

namespace gw2combat {

struct weapon {
    enum class type : std::uint32_t
    {
        EMPTY_HANDED,
        GREATSWORD,
        SWORD,
        FOCUS,
    };

    [[nodiscard]] inline double weapon_strength() const {
        return (weapon_strength_[0] + weapon_strength_[1]) / 2.0;
    }

    type type;
    double weapon_strength_[2];  // low = 0, high = 1
};

static inline weapon EMPTY_HANDED{weapon::type::EMPTY_HANDED, {656, 725}};
static inline weapon GREATSWORD{weapon::type::GREATSWORD, {1045, 1155}};
static inline weapon SWORD{weapon::type::SWORD, {950, 1050}};
static inline weapon FOCUS{weapon::type::FOCUS, {873, 927}};

}  // namespace gw2combat

#endif  // GW2COMBAT_WEAPON_HPP
