#ifndef GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP
#define GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP

#include "actor/effect.hpp"
#include "actor/unique_effect.hpp"

#include "condition.hpp"

namespace gw2combat::configuration {

struct effect_removal_t {
    condition_t condition;

    actor::effect_t effect = actor::effect_t::INVALID;
    actor::unique_effect_t unique_effect;
    // TODO: Implement all/num_stacks based removal
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(effect_removal_t, condition, effect, unique_effect)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_EFFECT_REMOVAL_HPP
