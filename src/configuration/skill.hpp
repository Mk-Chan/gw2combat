#ifndef GW2COMBAT_CONFIGURATION_SKILL_HPP
#define GW2COMBAT_CONFIGURATION_SKILL_HPP

#include "common.hpp"

#include "actor/bundle.hpp"
#include "actor/skill.hpp"

#include "attribute_conversion.hpp"
#include "attribute_modifier.hpp"
#include "condition.hpp"
#include "cooldown_modifier.hpp"
#include "counter_modifier.hpp"
#include "effect_application.hpp"
#include "effect_removal.hpp"
#include "weapon.hpp"

namespace gw2combat::configuration {

struct skill_tick_t {
    tick_t on_tick = 0;

    int num_targets = 1;
    actor::skill_t attribute_damage_to_skill;

    double flat_damage = 0.0;
    actor::weapon_type weapon_type =
        actor::weapon_type::INVALID;     // INVALID means the same weapon type as the skill cast.
    int weapon_strength_roll_group = 0;  // 0 means the same roll group as the skill cast.
    double damage_coefficient = 0.0;

    bool pulse = false;
    bool strike = false;
    bool can_critical_strike = true;

    // bool blast_finisher = false;
    // bool leap_finisher = false;
    // NOTE: Projectile finishers do DPS, so think about how to handle them.
    // bool projectile_finisher = false;
    bool whirl_finisher = false;

    std::vector<effect_application_t> on_strike_effect_applications{};
    std::vector<effect_application_t> on_pulse_effect_applications{};

    std::vector<attribute_modifier_t> attribute_modifiers{};
    std::vector<attribute_conversion_t> attribute_conversions{};
    std::vector<counter_modifier_t> counter_modifiers{};
    std::vector<skill_trigger_t> skill_triggers{};
    std::vector<skill_trigger_t> unchained_skill_triggers{};
    std::vector<skill_trigger_t> source_actor_skill_triggers{};
    std::vector<effect_removal_t> effect_removals{};
    std::vector<cooldown_modifier_t> cooldown_modifiers{};

    std::vector<actor::skill_t> skills_to_put_on_cooldown{};
    std::vector<actor::skill_t> skills_to_cancel{};

    std::vector<actor::skill_t> child_skill_keys;
    std::vector<actor::skill_tag_t> tags;
    bool inherit_tags = true;

    condition_t tick_condition{};
};

struct skill_t {
    actor::skill_t skill_key;
    std::vector<skill_tick_t> skill_ticks;
    actor::weapon_type weapon_type = actor::weapon_type::INVALID;
    actor::bundle_t required_bundle;

    actor::skill_t attribute_damage_to_skill;

    std::array<int, 2> cast_duration = {0, 0};
    std::array<int, 2> cooldown = {0, 0};

    double flat_damage = 0.0;
    double damage_coefficient = 0.0;
    int ammo = 1;
    int recharge_duration = 0;
    int num_targets = 1;

    std::array<std::vector<int>, 2> strike_on_tick_list = {std::vector<int>{}, std::vector<int>{}};
    std::array<std::vector<int>, 2> pulse_on_tick_list = {std::vector<int>{}, std::vector<int>{}};

    std::vector<effect_application_t> on_strike_effect_applications{};
    std::vector<effect_application_t> on_pulse_effect_applications{};

    std::vector<attribute_modifier_t> attribute_modifiers{};
    std::vector<attribute_conversion_t> attribute_conversions{};
    std::vector<counter_modifier_t> counter_modifiers{};
    std::vector<skill_trigger_t> skill_triggers{};
    std::vector<skill_trigger_t> unchained_skill_triggers{};
    std::vector<skill_trigger_t> source_actor_skill_triggers{};
    std::vector<effect_removal_t> effect_removals{};
    std::vector<cooldown_modifier_t> cooldown_modifiers{};

    std::vector<actor::skill_t> skills_to_put_on_cooldown{};
    std::vector<actor::skill_t> skills_to_cancel{};

    std::vector<actor::skill_t> child_skill_keys;
    std::vector<actor::skill_tag_t> tags;

    actor::combo_field_t combo_field = actor::combo_field_t::INVALID;
    // std::array<std::vector<int>, 2> blast_finisher_on_tick_list = {std::vector<int>{},
    //                                                                std::vector<int>{}};
    // std::array<std::vector<int>, 2> leap_finisher_on_tick_list = {std::vector<int>{},
    //                                                               std::vector<int>{}};
    // NOTE: Projectile finishers do DPS, so think about how to handle them.
    // std::array<std::vector<int>, 2> projectile_finisher_on_tick_list = {std::vector<int>{},
    //                                                                    std::vector<int>{}};
    std::array<std::vector<int>, 2> whirl_finisher_on_tick_list = {std::vector<int>{},
                                                                   std::vector<int>{}};

    bool instant_cast_only_when_not_in_animation = false;
    bool can_critical_strike = true;
    actor::bundle_t equip_bundle;
    actor::bundle_t drop_bundle;

    bool executable = false;

    condition_t cast_condition{};

    [[nodiscard]] inline bool operator==(const skill_t& rhs) const {
        return this->skill_key == rhs.skill_key;
    }
};

struct conditional_skill_t {
    condition_t condition;
    actor::skill_t skill_key;
};

struct conditional_skill_group_t {
    actor::skill_t skill_key;
    std::vector<conditional_skill_t> conditional_skill_keys;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_tick_t,
                                                on_tick,
                                                num_targets,
                                                attribute_damage_to_skill,
                                                flat_damage,
                                                weapon_type,
                                                weapon_strength_roll_group,
                                                damage_coefficient,
                                                pulse,
                                                strike,
                                                can_critical_strike,
                                                // blast_finisher,
                                                // leap_finisher,
                                                // projectile_finisher,
                                                whirl_finisher,
                                                on_strike_effect_applications,
                                                on_pulse_effect_applications,
                                                attribute_modifiers,
                                                attribute_conversions,
                                                counter_modifiers,
                                                skill_triggers,
                                                unchained_skill_triggers,
                                                source_actor_skill_triggers,
                                                effect_removals,
                                                cooldown_modifiers,
                                                skills_to_put_on_cooldown,
                                                skills_to_cancel,
                                                child_skill_keys,
                                                tags,
                                                inherit_tags,
                                                tick_condition)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_t,
                                                skill_key,
                                                skill_ticks,
                                                weapon_type,
                                                required_bundle,
                                                attribute_damage_to_skill,
                                                cast_duration,
                                                cooldown,
                                                flat_damage,
                                                damage_coefficient,
                                                ammo,
                                                recharge_duration,
                                                num_targets,
                                                strike_on_tick_list,
                                                pulse_on_tick_list,
                                                on_strike_effect_applications,
                                                on_pulse_effect_applications,
                                                attribute_conversions,
                                                attribute_modifiers,
                                                counter_modifiers,
                                                skill_triggers,
                                                unchained_skill_triggers,
                                                source_actor_skill_triggers,
                                                effect_removals,
                                                cooldown_modifiers,
                                                skills_to_put_on_cooldown,
                                                skills_to_cancel,
                                                child_skill_keys,
                                                tags,
                                                combo_field,
                                                // blast_finisher_on_tick_list,
                                                // leap_finisher_on_tick_list,
                                                // projectile_finisher_on_tick_list,
                                                whirl_finisher_on_tick_list,
                                                instant_cast_only_when_not_in_animation,
                                                can_critical_strike,
                                                equip_bundle,
                                                drop_bundle,
                                                executable,
                                                cast_condition)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(conditional_skill_t, condition, skill_key)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(conditional_skill_group_t,
                                                skill_key,
                                                conditional_skill_keys)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_SKILL_HPP
