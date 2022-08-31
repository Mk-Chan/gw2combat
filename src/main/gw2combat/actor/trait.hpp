#ifndef GW2COMBAT_TRAIT_HPP
#define GW2COMBAT_TRAIT_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/actor/base_class.hpp"

namespace gw2combat::actor {

enum class trait_line_t : std::uint32_t
{
    INVALID,

    RADIANCE,
    VIRTUES,
    ZEAL,
    FIREBRAND,
    DRAGONHUNTER,
};

struct trait_t {
    base_class_t base_class = base_class_t::INVALID;
    std::string name;

    [[nodiscard]] constexpr inline bool operator==(const trait_t& rhs) const {
        return (this->base_class == base_class_t::UNIVERSAL ||
                rhs.base_class == base_class_t::UNIVERSAL || this->base_class == rhs.base_class) &&
               this->name == rhs.name;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(trait_t, base_class, name)
NLOHMANN_JSON_SERIALIZE_ENUM(trait_line_t,
                             {
                                 {trait_line_t::INVALID, "Invalid"},

                                 {trait_line_t::ZEAL, "Zeal"},
                                 {trait_line_t::RADIANCE, "Radiance"},
                                 {trait_line_t::VIRTUES, "Virtues"},
                                 {trait_line_t::FIREBRAND, "Firebrand"},
                                 {trait_line_t::DRAGONHUNTER, "Dragonhunter"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_TRAIT_HPP
