#ifndef GW2COMBAT_EFFECT_APPLICATION_HPP
#define GW2COMBAT_EFFECT_APPLICATION_HPP

#include "gw2combat/common.hpp"

#include "effect.hpp"

namespace gw2combat::effect {

enum class direction_t : std::uint8_t
{
    INVALID,

    OUTGOING,
    INCOMING,
    TEAM,
};

struct application_t {
    [[nodiscard]] application_t() = default;
    [[nodiscard]] application_t(effect_t effect_type, direction_t direction, int base_duration)
        : effect_type(effect_type),
          direction(direction),
          base_duration(base_duration),
          num_stacks(1) {
    }
    [[nodiscard]] application_t(effect_t effect_type,
                                direction_t direction,
                                int base_duration,
                                int num_stacks)
        : effect_type(effect_type),
          direction(direction),
          base_duration(base_duration),
          num_stacks(num_stacks) {
    }

    effect_t effect_type;
    direction_t direction;
    int base_duration;
    int num_stacks;
};

NLOHMANN_JSON_SERIALIZE_ENUM(direction_t,
                             {
                                 {direction_t::INVALID, "Invalid"},

                                 {direction_t::OUTGOING, "OUTGOING"},
                                 {direction_t::INCOMING, "INCOMING"},
                                 {direction_t::TEAM, "TEAM"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(application_t,
                                                effect_type,
                                                direction,
                                                base_duration,
                                                num_stacks)

}  // namespace gw2combat::effect

#endif  // GW2COMBAT_EFFECT_APPLICATION_HPP
