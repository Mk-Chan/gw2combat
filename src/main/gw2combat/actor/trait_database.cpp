#include "trait_database.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include <fstream>

namespace gw2combat::actor {

trait_database& trait_database::instance() {
    static trait_database database;
    static bool initialized = false;

    if (!initialized) {
        const auto file_path = std::filesystem::path("traits.json");
        std::ifstream ifstream{file_path, std::ios::in};
        nlohmann::json build_json = nlohmann::json::parse(ifstream);
        database = build_json.get<trait_database>();
        initialized = true;
    }

    return database;
}

const trait_configuration_t& trait_database::find_by(trait_t trait) const {
    auto iter = std::find_if(trait_configurations.cbegin(),
                             trait_configurations.cend(),
                             [&](const trait_configuration_t& trait_configuration) {
                                 return trait_configuration.trait_key == trait;
                             });
    if (iter == trait_configurations.cend()) {
        throw std::runtime_error(
            fmt::format("trait {} not found in database!", utils::to_string(trait)));
    }
    return *iter;
}

}  // namespace gw2combat::actor
