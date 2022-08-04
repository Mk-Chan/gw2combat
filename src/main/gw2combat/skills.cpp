#include "skills.hpp"

#include <fstream>

using json = nlohmann::json;

namespace gw2combat::skills {

skills_db SKILLS_DB;

void skills_db::init(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    json skills_json = json::parse(ifstream);
    *this = skills_json.get<skills_db>();

    skills::skill weapon_swap_skill;
    weapon_swap_skill.name = "Weapon Swap";
    weapon_swap_skill.cast_duration = {1, 1};
    this->skills.emplace_back(weapon_swap_skill);
}

std::optional<skill> skills_db::get_by_name(const std::string& name) const {
    auto skill_iter = std::find_if(
        skills.begin(), skills.end(), [&](const skill& skill) { return skill.name == name; });
    if (skill_iter != skills.end()) {
        return *skill_iter;
    } else {
        return std::nullopt;
    }
}

}  // namespace gw2combat::skills
