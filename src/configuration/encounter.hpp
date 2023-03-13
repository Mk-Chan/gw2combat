#ifndef GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
#define GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP

#include "common.hpp"

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
    };
    type_t type = type_t::INVALID;
    tick_t time = 0;
    std::string actor;
    int damage = 0;
};

struct encounter_t {
    std::vector<configuration::actor_t> actors;
    std::vector<termination_condition_t> termination_conditions;
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
                              {termination_condition_t::type_t::DAMAGE, "DAMAGE"}})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(termination_condition_t, type, time, actor, damage)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(encounter_t, actors, termination_conditions)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
