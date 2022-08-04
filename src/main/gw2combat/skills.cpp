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
    this->skills.emplace_back(skills::skill{"Weapon Swap"});
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
