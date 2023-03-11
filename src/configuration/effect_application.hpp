#ifndef GW2COMBAT_CONFIGURATION_EFFECT_APPLICATION_HPP
#define GW2COMBAT_CONFIGURATION_EFFECT_APPLICATION_HPP

#include "actor/effect.hpp"
#include "actor/unique_effect.hpp"

#include "condition.hpp"
#include "unique_effect.hpp"

namespace gw2combat::configuration {

enum class direction_t : std::uint8_t
{
    INVALID,

    SELF,
    TEAM,
    OUTGOING,
};

struct effect_application_t {
    condition_t condition;

    actor::effect_t effect = actor::effect_t::INVALID;
    configuration::unique_effect_t unique_effect;
    direction_t direction = direction_t::INVALID;
    int base_duration_ms = 0;
    int num_stacks = 1;
    int num_targets = 1;
};

NLOHMANN_JSON_SERIALIZE_ENUM(direction_t,
                             {
                                 {direction_t::INVALID, "invalid"},

                                 {direction_t::SELF, "SELF"},
                                 {direction_t::TEAM, "TEAM"},
                                 {direction_t::OUTGOING, "OUTGOING"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_application_t,
                                                condition,
                                                effect,
                                                unique_effect,
                                                direction,
                                                base_duration_ms,
                                                num_stacks,
                                                num_targets)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_EFFECT_APPLICATION_HPP
