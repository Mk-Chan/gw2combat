#ifndef GW2COMBAT_SKILL_HPP
#define GW2COMBAT_SKILL_HPP

#include <optional>
#include <string>

#include "effects.hpp"
#include "types.hpp"
#include "weapon.hpp"

namespace gw2combat::skills {

enum class skill_tag : std::uint32_t
{
    WHIRL_FINISHER,
    LEAP_FINISHER,
    BLAST_FINISHER,

    LIGHT_FIELD,

    SYMBOL,
    SPIRIT_WEAPON,
};

enum class applied_effect_direction : std::uint8_t
{
    OUTGOING,
    INCOMING,
};

struct effect_application {
    effects::applied_effect_type effect_type;
    applied_effect_direction effect_direction;
    size_t num_stacks;
    tick_t duration;
};

struct skill {
    std::string name;
    weapon_type weapon_type;
    double damage_coefficient;
    std::array<tick_t, 2> cast_duration;
    std::array<std::vector<tick_t>, 2> hit_on_tick_list;
    std::vector<effect_application> on_hit_effect_applications;
    std::array<std::vector<tick_t>, 2> pulse_on_tick_list;
    std::vector<effect_application> on_pulse_effect_applications;
    std::vector<skill> child_entity_skills;
    std::vector<skill_tag> tags;

    inline bool operator==(const skill& rhs) const {
        return this->name == rhs.name;
    }
};

struct skills_db {
    void init(const std::string& path);
    [[nodiscard]] std::optional<skill> get_by_name(const std::string& name) const;

    std::vector<skill> skills;
};

extern skills_db SKILLS_DB;

NLOHMANN_JSON_SERIALIZE_ENUM(skill_tag,
                             {
                                 {skill_tag::WHIRL_FINISHER, "WHIRL_FINISHER"},
                                 {skill_tag::LEAP_FINISHER, "LEAP_FINISHER"},
                                 {skill_tag::BLAST_FINISHER, "BLAST_FINISHER"},

                                 {skill_tag::LIGHT_FIELD, "LIGHT_FIELD"},

                                 {skill_tag::SYMBOL, "SYMBOL"},
                                 {skill_tag::SPIRIT_WEAPON, "SPIRIT_WEAPON"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(applied_effect_direction,
                             {
                                 {applied_effect_direction::OUTGOING, "OUTGOING"},
                                 {applied_effect_direction::INCOMING, "INCOMING"},
                             })

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(effect_application,
                                   effect_type,
                                   effect_direction,
                                   num_stacks,
                                   duration)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(skill,
                                   name,
                                   weapon_type,
                                   damage_coefficient,
                                   cast_duration,
                                   hit_on_tick_list,
                                   on_hit_effect_applications,
                                   pulse_on_tick_list,
                                   on_pulse_effect_applications,
                                   child_entity_skills,
                                   tags)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(skills_db, skills)

}  // namespace gw2combat::skills

#endif  // GW2COMBAT_SKILL_HPP
