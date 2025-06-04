#ifndef GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
#define GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP

#include "common.hpp"

#include "configuration/audit.hpp"
#include "configuration/build.hpp"
#include "configuration/rotation.hpp"

namespace gw2combat::configuration {

struct actor_t {
    std::string name;
    configuration::build_t build;
    configuration::rotation_t rotation;
    int team = 0;

    std::string audit_base_path;
};

struct termination_condition_t {
    enum class type_t
    {
        INVALID,

        TIME,
        ROTATION,
        DAMAGE,
        NO_ACTIVE_SKILLS,
        NO_MORE_ROTATION,
    };
    type_t type = type_t::INVALID;
    tick_t time = 0;
    std::string actor;
    int damage = 0;
};

enum class weapon_strength_mode_t
{
    MEAN,
    RANDOM_UNIFORM,
    LOWEST,
    HIGHEST,
};

enum class critical_strike_mode_t
{
    MEAN,
    RANDOM_UNIFORM,
};

struct encounter_t {
    std::vector<configuration::actor_t> actors;
    std::vector<termination_condition_t> termination_conditions;
    audit_t audit_configuration;
    bool require_afk_skills = false;
    int condition_tick_offset = 0;
    int audit_offset = 0;
    weapon_strength_mode_t weapon_strength_mode = weapon_strength_mode_t::MEAN;
    critical_strike_mode_t critical_strike_mode = critical_strike_mode_t::MEAN;
    bool enable_caching = true;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_t,
                                                name,
                                                build,
                                                rotation,
                                                team,
                                                audit_base_path)
NLOHMANN_JSON_SERIALIZE_ENUM(termination_condition_t::type_t,
                             {{termination_condition_t::type_t::INVALID, "invalid"},
                              {termination_condition_t::type_t::TIME, "TIME"},
                             {termination_condition_t::type_t::ROTATION, "ROTATION"},
                             {termination_condition_t::type_t::DAMAGE, "DAMAGE"},
                             {termination_condition_t::type_t::NO_ACTIVE_SKILLS, "NO_ACTIVE_SKILLS"},
                             {termination_condition_t::type_t::NO_MORE_ROTATION, "NO_MORE_ROTATION"}})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(termination_condition_t, type, time, actor, damage)
NLOHMANN_JSON_SERIALIZE_ENUM(weapon_strength_mode_t,
                             {{weapon_strength_mode_t::MEAN, "MEAN"},
                              {weapon_strength_mode_t::RANDOM_UNIFORM, "RANDOM"},
                              {weapon_strength_mode_t::LOWEST, "LOWEST"},
                              {weapon_strength_mode_t::HIGHEST, "HIGHEST"}})
NLOHMANN_JSON_SERIALIZE_ENUM(critical_strike_mode_t,
                             {{critical_strike_mode_t::MEAN, "MEAN"},
                              {critical_strike_mode_t::RANDOM_UNIFORM, "RANDOM"}})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(encounter_t,
                                                actors,
                                                termination_conditions,
                                                audit_configuration,
                                                require_afk_skills,
                                                condition_tick_offset,
                                                audit_offset,
                                                weapon_strength_mode,
                                                critical_strike_mode,
                                                enable_caching)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
