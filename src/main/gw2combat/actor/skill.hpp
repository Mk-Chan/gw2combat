#ifndef GW2COMBAT_SKILL_HPP
#define GW2COMBAT_SKILL_HPP

#include "gw2combat/common.hpp"

#include "gw2combat/component/attribute_modifications.hpp"
#include "gw2combat/effect/application.hpp"

#include "base_class.hpp"
#include "weapon.hpp"

namespace gw2combat::actor {

enum class skill_tag_t : std::uint32_t
{
    INVALID,

    CANNOT_CRITICAL_HIT,
    CONSECRATION,
    TRAP,
    MANTRA,
    SYMBOL,
    SPIRIT_WEAPON,
    GUARDIAN_VIRTUE,
    TOME,
};

struct skill_t {
    base_class_t base_class;
    std::string name;

    constexpr inline bool operator==(const skill_t& rhs) const {
        return (this->base_class == base_class_t::UNIVERSAL ||
                this->base_class == rhs.base_class) &&
               this->name == rhs.name;
    }
};

struct skill_configuration_t {
    skill_t skill_key;
    bool is_child_skill = false;

    weapon_type weapon_type;
    weapon_position weapon_position = weapon_position::UNIVERSAL;
    double damage_coefficient;

    std::array<int, 2> cast_duration;
    std::array<int, 2> cooldown;

    std::array<std::vector<int>, 2> hit_on_tick_list;
    std::array<std::vector<int>, 2> pulse_on_tick_list;

    std::vector<effect::application_t> on_hit_effect_applications;
    std::vector<effect::application_t> on_pulse_effect_applications;

    std::vector<skill_t> child_skill_keys;
    std::vector<skill_tag_t> tags;
    std::vector<component::attribute_modification_t> attribute_modifications;

    int ammo = 1;
    int recharge_duration = 0;

    constexpr inline bool operator==(const skill_configuration_t& rhs) const {
        return this->skill_key == rhs.skill_key;
    }
};

struct skill_database {
    [[nodiscard]] static skill_database& instance();
    [[nodiscard]] const skill_configuration_t& find_by(const skill_t& skill) const;
    [[nodiscard]] std::vector<skill_configuration_t> find_by(
        const weapon_type& weapon_type,
        const weapon_position& weapon_position) const;

    std::vector<skill_configuration_t> skill_configurations;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_t, base_class, name)
NLOHMANN_JSON_SERIALIZE_ENUM(skill_tag_t,
                             {
                                 {skill_tag_t::INVALID, "Invalid"},

                                 {skill_tag_t::CANNOT_CRITICAL_HIT, "CANNOT_CRITICAL_HIT"},
                                 {skill_tag_t::CONSECRATION, "CONSECRATION"},
                                 {skill_tag_t::TRAP, "TRAP"},
                                 {skill_tag_t::MANTRA, "MANTRA"},
                                 {skill_tag_t::SYMBOL, "SYMBOL"},
                                 {skill_tag_t::SPIRIT_WEAPON, "SPIRIT_WEAPON"},
                                 {skill_tag_t::GUARDIAN_VIRTUE, "GUARDIAN_VIRTUE"},
                                 {skill_tag_t::TOME, "TOME"},
                             })
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_configuration_t,
                                                skill_key,
                                                is_child_skill,
                                                weapon_type,
                                                weapon_position,
                                                damage_coefficient,
                                                cast_duration,
                                                cooldown,
                                                hit_on_tick_list,
                                                pulse_on_tick_list,
                                                on_hit_effect_applications,
                                                on_pulse_effect_applications,
                                                child_skill_keys,
                                                tags,
                                                attribute_modifications,
                                                ammo,
                                                recharge_duration)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_database, skill_configurations)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_SKILL_HPP
