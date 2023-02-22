#ifndef GW2COMBAT_CONFIGURATION_ENCOUNTER_SERVER_HPP
#define GW2COMBAT_CONFIGURATION_ENCOUNTER_SERVER_HPP

#include "common.hpp"

#include "configuration/build.hpp"
#include "configuration/rotation.hpp"

namespace gw2combat::configuration {

struct actor_server_t {
    std::string name;
    configuration::build_t build;
    configuration::rotation_t rotation;
    int team = 0;
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
    int value = 0;
};

struct encounter_server_t {
    std::vector<configuration::actor_server_t> actors;
    std::vector<termination_condition_t> termination_conditions;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_server_t, name, build, rotation, team)
NLOHMANN_JSON_SERIALIZE_ENUM(termination_condition_t::type_t,
                             {{termination_condition_t::type_t::INVALID, "invalid"},
                              {termination_condition_t::type_t::TIME, "TIME"},
                              {termination_condition_t::type_t::ROTATION, "ROTATION"},
                              {termination_condition_t::type_t::DAMAGE, "DAMAGE"}})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(termination_condition_t, type, value)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(encounter_server_t, actors, termination_conditions)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ENCOUNTER_SERVER_HPP
