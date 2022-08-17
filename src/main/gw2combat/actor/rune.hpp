#ifndef GW2COMBAT_RUNE_HPP
#define GW2COMBAT_RUNE_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::actor {

enum class rune_t : std::uint32_t
{
    INVALID,

    BALTHAZAR,
    SCHOLAR,
};

NLOHMANN_JSON_SERIALIZE_ENUM(rune_t,
                             {
                                 {rune_t::INVALID, "Invalid"},

                                 {rune_t::BALTHAZAR, "Rune of Balthazar"},
                                 {rune_t::SCHOLAR, "Rune of Scholar"},

                                 {rune_t::BALTHAZAR, "balthazar"},
                                 {rune_t::SCHOLAR, "scholar"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_RUNE_HPP
