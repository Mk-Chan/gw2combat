#include "unique_effect_database.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include <fstream>

namespace gw2combat::actor {

unique_effect_database& unique_effect_database::instance() {
    static unique_effect_database database;
    static bool initialized = false;

    if (!initialized) {
        const auto file_path = std::filesystem::path("unique_effects.json");
        std::ifstream ifstream{file_path, std::ios::in};
        nlohmann::json build_json = nlohmann::json::parse(ifstream);
        database = build_json.get<unique_effect_database>();
        initialized = true;
    }

    return database;
}

const unique_effect_configuration_t& unique_effect_database::find_by(unique_effect_t effect) const {
    auto iter = std::find_if(unique_effect_configurations.cbegin(),
                             unique_effect_configurations.cend(),
                             [&](const unique_effect_configuration_t& effect_configuration) {
                                 return effect_configuration.unique_effect_key == effect;
                             });
    if (iter == unique_effect_configurations.cend()) {
        throw std::runtime_error(
            fmt::format("unique effect {} not found in database!", utils::to_string(effect)));
    }
    return *iter;
}

}  // namespace gw2combat::actor
