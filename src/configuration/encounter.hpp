#ifndef GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
#define GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP

#include "common.hpp"

namespace gw2combat::configuration {

struct actor_t {
    std::string name;
    std::string build_path;
    std::string rotation_path;
    int team = 0;
};

struct encounter_t {
    std::vector<actor_t> actors;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(actor_t, name, build_path, rotation_path, team)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(encounter_t, actors)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ENCOUNTER_HPP
