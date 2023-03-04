#ifndef GW2COMBAT_CONFIGURATION_UNIQUE_EFFECT_HPP
#define GW2COMBAT_CONFIGURATION_UNIQUE_EFFECT_HPP

#include <vector>

#include "actor/unique_effect.hpp"

#include "attribute_conversion.hpp"
#include "attribute_modifier.hpp"
#include "effect_removal.hpp"
#include "skill_trigger.hpp"

namespace gw2combat::configuration {

struct unique_effect_t {
    actor::unique_effect_t unique_effect_key;

    std::vector<attribute_modifier_t> attribute_modifiers;
    std::vector<attribute_conversion_t> attribute_conversions;
    std::vector<skill_trigger_t> skill_triggers;
    std::vector<skill_trigger_t> unchained_skill_triggers;
    std::vector<effect_removal_t> effect_removals;

    int max_considered_stacks = 1;
    int max_stored_stacks = 1500;

    [[nodiscard]] inline bool operator==(const unique_effect_t& rhs) const {
        return this->unique_effect_key == rhs.unique_effect_key;
    }
    [[nodiscard]] inline bool is_invalid() const {
        return unique_effect_key.empty();
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effect_t,
                                                unique_effect_key,
                                                attribute_modifiers,
                                                attribute_conversions,
                                                skill_triggers,
                                                unchained_skill_triggers,
                                                effect_removals,
                                                max_considered_stacks,
                                                max_stored_stacks)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_UNIQUE_EFFECT_HPP
