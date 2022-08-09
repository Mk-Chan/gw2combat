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

    if (!get_by_name("Weapon Swap")) {
        skills::skill weapon_swap_skill;
        weapon_swap_skill.name = "Weapon Swap";
        weapon_swap_skill.cast_duration = {100, 100};
        this->skills.emplace_back(weapon_swap_skill);
    }
}

std::optional<skill> skills_db::get_by_name(const std::string& name) const {
    auto skill_iter =
        ranges::find_if(skills, [&](const skill& skill) { return skill.name == name; });
    if (skill_iter != ranges::end(skills)) {
        return *skill_iter;
    } else {
        return std::nullopt;
    }
}

}  // namespace gw2combat::skills
