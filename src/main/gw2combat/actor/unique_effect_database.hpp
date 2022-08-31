#ifndef GW2COMBAT_ACTOR_UNIQUE_EFFECT_DATABASE_HPP
#define GW2COMBAT_ACTOR_UNIQUE_EFFECT_DATABASE_HPP

#include "unique_effect.hpp"

#include "gw2combat/component/attribute_conversions_component.hpp"
#include "gw2combat/component/attribute_modifiers_component.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/skill_triggers_component.hpp"

namespace gw2combat::actor {

struct unique_effect_configuration_t {
    unique_effect_t unique_effect_key;

    std::vector<component::attribute_modifiers_t> attribute_modifiers;
    std::vector<component::attribute_conversions_t> attribute_conversions;
    std::vector<component::damage_modifiers_t> damage_modifiers;
    std::vector<component::skill_triggers_t> skill_triggers;

    size_t max_considered_stacks = 1;
    size_t max_stored_stacks = 1500;

    [[nodiscard]] constexpr inline bool operator==(const unique_effect_configuration_t& rhs) const {
        return this->unique_effect_key == rhs.unique_effect_key;
    }
};

struct unique_effect_database {
    [[nodiscard]] static unique_effect_database& instance();
    [[nodiscard]] const unique_effect_configuration_t& find_by(unique_effect_t effect) const;

    std::vector<unique_effect_configuration_t> unique_effect_configurations;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effect_configuration_t,
                                                unique_effect_key,
                                                attribute_modifiers,
                                                attribute_conversions,
                                                damage_modifiers,
                                                skill_triggers,

                                                max_considered_stacks)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(unique_effect_database,
                                                unique_effect_configurations)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_UNIQUE_EFFECT_DATABASE_HPP
