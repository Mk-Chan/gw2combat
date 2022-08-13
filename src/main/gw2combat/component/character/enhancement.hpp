#ifndef GW2COMBAT_COMPONENT_CHARACTER_NOURISHMENT_HPP
#define GW2COMBAT_COMPONENT_CHARACTER_NOURISHMENT_HPP

#include "gw2combat/types.hpp"

namespace gw2combat::component {

enum class enhancement_type : std::uint8_t
{
    TOXIC_FOCUSING_CRYSTAL,
    SUPERIOR_SHARPENING_STONE,
};

struct enhancement_component {
    enhancement_type enhancement;
};

NLOHMANN_JSON_SERIALIZE_ENUM(
    enhancement_type,
    {
        {enhancement_type::TOXIC_FOCUSING_CRYSTAL, "toxic-focusing-crystal"},
        {enhancement_type::SUPERIOR_SHARPENING_STONE, "superior-sharpening-stone"},
    })

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_CHARACTER_NOURISHMENT_HPP
