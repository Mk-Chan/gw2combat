#ifndef GW2COMBAT_ACTOR_UNIQUE_EFFECT_HPP
#define GW2COMBAT_ACTOR_UNIQUE_EFFECT_HPP

#include "common.hpp"

namespace gw2combat::actor {

struct unique_effect_t {
    std::string name;

    [[nodiscard]] constexpr inline bool is_invalid() const {
        return name.empty();
    }

    [[nodiscard]] constexpr inline bool operator==(const unique_effect_t& rhs) const {
        return this->name == rhs.name;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effect_t, name)

}  // namespace gw2combat::actor

namespace std {

template <>
struct hash<gw2combat::actor::unique_effect_t> {
    std::size_t operator()(const gw2combat::actor::unique_effect_t& unique_effect) const {
        return std::hash<std::string>()(unique_effect.name);
    }
};

}  // namespace std

#endif  // GW2COMBAT_ACTOR_UNIQUE_EFFECT_HPP
