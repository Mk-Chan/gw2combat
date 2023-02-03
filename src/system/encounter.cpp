#include "encounter.hpp"

#include "configuration/build.hpp"
#include "configuration/encounter.hpp"

namespace gw2combat::system {

void encounter(registry_t& registry) {
    auto encounter = utils::read<configuration::encounter_t>("resources/encounter.json");
    for (auto&& actor : encounter.actors) {
        auto build = utils::read<configuration::build_t>(actor.build_path);
        spdlog::info("{}: {}", actor.name, utils::to_string(build));
    }
}

}  // namespace gw2combat::system
