#ifndef GW2COMBAT_NOURISHMENT_ENHANCEMENT_HPP
#define GW2COMBAT_NOURISHMENT_ENHANCEMENT_HPP

#include "gw2combat/common.hpp"

namespace gw2combat::actor {

enum class nourishment_t : std::uint8_t
{
    INVALID,

    BOWL_OF_SWEET_AND_SPICY_BUTTERNUT_SQUASH_SOUP,
    PLATE_OF_BEEF_RENDANG,
};

enum class enhancement_t : std::uint8_t
{
    INVALID,

    TOXIC_FOCUSING_CRYSTAL,
    SUPERIOR_SHARPENING_STONE,
};

NLOHMANN_JSON_SERIALIZE_ENUM(nourishment_t,
                             {
                                 {nourishment_t::INVALID, "Invalid"},

                                 {nourishment_t::BOWL_OF_SWEET_AND_SPICY_BUTTERNUT_SQUASH_SOUP,
                                  "sweet-and-spicy-butternut-squash-soup"},
                                 {nourishment_t::PLATE_OF_BEEF_RENDANG, "beef-rendang"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(enhancement_t,
                             {
                                 {enhancement_t::INVALID, "Invalid"},

                                 {enhancement_t::TOXIC_FOCUSING_CRYSTAL, "toxic-focusing-crystal"},
                                 {enhancement_t::SUPERIOR_SHARPENING_STONE,
                                  "superior-sharpening-stone"},
                             })

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_NOURISHMENT_ENHANCEMENT_HPP
