#ifndef GW2COMBAT_ACTOR_SKILL_DATABASE_HPP
#define GW2COMBAT_ACTOR_SKILL_DATABASE_HPP

#include "skill.hpp"

#include "base_class.hpp"
#include "weapon.hpp"

#include "gw2combat/actor/attributes.hpp"
#include "gw2combat/actor/trait.hpp"
#include "gw2combat/component/equipment/bundle.hpp"
#include "gw2combat/effect/application.hpp"
#include "gw2combat/modifiers/attribute_modifiers.hpp"
#include "gw2combat/modifiers/damage_modifiers.hpp"

namespace gw2combat::actor {

struct skill_configuration_t {
    skill_t skill_key;
    bool is_child_skill = false;

    weapon_type weapon_type = weapon_type::INVALID;
    weapon_position weapon_position = weapon_position::UNIVERSAL;
    double damage_coefficient = 0.0;

    std::array<int, 2> cast_duration = {0, 0};
    std::array<int, 2> cooldown = {0, 0};

    std::array<std::vector<int>, 2> strike_on_tick_list = {std::vector<int>{}, std::vector<int>{}};
    std::array<std::vector<int>, 2> pulse_on_tick_list = {std::vector<int>{}, std::vector<int>{}};

    std::vector<effect::application_t> on_strike_effect_applications;
    std::vector<effect::application_t> on_pulse_effect_applications;

    std::vector<skill_t> child_skill_keys;
    std::unordered_set<skill_tag_t> tags;

    modifiers::attribute_modifiers_t attribute_modifiers;
    modifiers::damage_modifiers_t damage_modifiers;

    component::bundle_component equip_bundle;

    int ammo = 1;
    int recharge_duration = 0;
    int num_targets = 1;

    [[nodiscard]] constexpr inline bool operator==(const skill_configuration_t& rhs) const {
        return this->skill_key == rhs.skill_key;
    }
};

struct skill_database {
    [[nodiscard]] static skill_database copy();
    [[nodiscard]] const skill_configuration_t& find_by(const skill_t& skill) const;
    [[nodiscard]] std::vector<skill_configuration_t> find_by(
        const weapon_type& weapon_type,
        const weapon_position& weapon_position) const;
    [[nodiscard]] std::vector<skill_configuration_t> find_by(const skill_tag_t& skill_tag) const;

    std::vector<skill_configuration_t> skill_configurations;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_configuration_t,
                                                skill_key,
                                                is_child_skill,
                                                weapon_type,
                                                weapon_position,
                                                damage_coefficient,
                                                cast_duration,
                                                cooldown,
                                                strike_on_tick_list,
                                                pulse_on_tick_list,
                                                on_strike_effect_applications,
                                                on_pulse_effect_applications,
                                                child_skill_keys,
                                                tags,
                                                attribute_modifiers,
                                                damage_modifiers,
                                                equip_bundle,
                                                ammo,
                                                recharge_duration,
                                                num_targets)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_database, skill_configurations)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_SKILL_DATABASE_HPP
