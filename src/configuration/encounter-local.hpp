#ifndef GW2COMBAT_CONFIGURATION_ENCOUNTER_LOCAL_HPP
#define GW2COMBAT_CONFIGURATION_ENCOUNTER_LOCAL_HPP

#include "common.hpp"

#include "encounter.hpp"

namespace gw2combat::configuration {

struct actor_local_t {
    std::string name;
    std::string build_path;
    std::string rotation_path;
    int team = 0;

    std::string audit_base_path;
};

struct encounter_local_t {
    std::vector<actor_local_t> actors;
    std::vector<termination_condition_t> termination_conditions;
    audit_t audit_configuration;
    bool require_afk_skills = false;
    int audit_offset = 0;
    weapon_strength_mode_t weapon_strength_mode = weapon_strength_mode_t::MEAN;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_local_t,
                                                name,
                                                build_path,
                                                rotation_path,
                                                team,
                                                audit_base_path)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(encounter_local_t,
                                                actors,
                                                termination_conditions,
                                                audit_configuration,
                                                require_afk_skills,
                                                audit_offset,
                                                weapon_strength_mode)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ENCOUNTER_LOCAL_HPP
