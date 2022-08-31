#include "skill_database.hpp"

#include "gw2combat/utilities/base_utilities.hpp"

#include <fstream>

namespace gw2combat::actor {

skill_database skill_database::copy() {
    static skill_database database;
    static bool initialized = false;

    if (!initialized) {
        const auto file_path = std::filesystem::path("skills.json");
        std::ifstream ifstream{file_path, std::ios::in};
        nlohmann::json build_json = nlohmann::json::parse(ifstream);
        database = build_json.get<skill_database>();
        initialized = true;
    }

    return database;
}

const skill_configuration_t& skill_database::find_by(const skill_t& skill) const {
    auto iter = std::find_if(skill_configurations.cbegin(),
                             skill_configurations.cend(),
                             [&](const skill_configuration_t& skill_configuration) {
                                 return skill_configuration.skill_key == skill;
                             });
    if (iter == skill_configurations.cend()) {
        throw std::runtime_error(
            fmt::format("skill {} not found in database!", utils::to_string(skill)));
    }
    return *iter;
}

std::vector<skill_configuration_t> skill_database::find_by(
    const weapon_type& weapon_type,
    const weapon_position& weapon_position) const {
    std::vector<skill_configuration_t> weapon_skills;
    std::copy_if(skill_configurations.cbegin(),
                 skill_configurations.cend(),
                 std::back_inserter(weapon_skills),
                 [&](const skill_configuration_t& skill_configuration) {
                     return skill_configuration.weapon_type == weapon_type &&
                            (skill_configuration.weapon_position == weapon_position::UNIVERSAL ||
                             skill_configuration.weapon_position == weapon_position);
                 });
    return weapon_skills;
}

std::vector<skill_configuration_t> skill_database::find_by(const skill_tag_t& skill_tag) const {
    std::vector<skill_configuration_t> tagged_skills;
    std::copy_if(skill_configurations.cbegin(),
                 skill_configurations.cend(),
                 std::back_inserter(tagged_skills),
                 [&](const skill_configuration_t& skill_configuration) {
                     return skill_configuration.tags.contains(skill_tag);
                 });
    return tagged_skills;
}

}  // namespace gw2combat::actor
