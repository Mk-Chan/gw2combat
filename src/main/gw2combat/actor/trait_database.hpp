#ifndef GW2COMBAT_ACTOR_TRAIT_DATABASE_HPP
#define GW2COMBAT_ACTOR_TRAIT_DATABASE_HPP

#include "trait.hpp"

#include "gw2combat/component/attribute_conversions_component.hpp"
#include "gw2combat/component/attribute_modifiers_component.hpp"
#include "gw2combat/component/damage_modifiers_component.hpp"
#include "gw2combat/component/skill_modifiers_component.hpp"

namespace gw2combat::actor {

struct trait_configuration_t {
    trait_t trait_key;

    std::vector<component::attribute_modifiers_t> attribute_modifiers;
    std::vector<component::attribute_conversions_t> attribute_conversions;
    std::vector<component::damage_modifiers_t> damage_modifiers;
    std::vector<component::skill_modifiers_t> skill_modifiers;

    [[nodiscard]] constexpr inline bool operator==(const trait_configuration_t& rhs) const {
        return this->trait_key == rhs.trait_key;
    }
};

struct trait_database {
    [[nodiscard]] static trait_database& instance();
    [[nodiscard]] const trait_configuration_t& find_by(trait_t trait) const;

    std::vector<trait_configuration_t> trait_configurations;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(trait_configuration_t,
                                                trait_key,
                                                attribute_modifiers,
                                                attribute_conversions,
                                                damage_modifiers,
                                                skill_modifiers)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(trait_database, trait_configurations)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_TRAIT_DATABASE_HPP
