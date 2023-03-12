#ifndef GW2COMBAT_ACTOR_PROFESSION_HPP
#define GW2COMBAT_ACTOR_PROFESSION_HPP

#include "common.hpp"

namespace gw2combat::actor {

enum class profession_t : std::uint8_t
{
    INVALID,

    DRAGONHUNTER,
    SOULBEAST,
};

NLOHMANN_JSON_SERIALIZE_ENUM(profession_t,
                             {
                                 {profession_t::INVALID, "invalid"},
                                 {profession_t::DRAGONHUNTER, "dragonhunter"},
                                 {profession_t::SOULBEAST, "soulbeast"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_PROFESSION_HPP
