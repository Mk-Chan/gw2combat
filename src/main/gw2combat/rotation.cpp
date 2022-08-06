#include "rotation.hpp"

#include <spdlog/spdlog.h>
#include <exception>
#include <filesystem>

#include "rapidcsv.h"

namespace gw2combat {

std::vector<skill_cast> read_scraped_simple_rotation(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    const auto file_size = std::filesystem::file_size(file_path);

    std::string csv(file_size, '\0');

    ifstream.read(csv.data(), file_size);

    std::stringstream csv_stream{csv};
    rapidcsv::Document doc{csv_stream};

    std::vector<skill_cast> rotation;

    int offset;
    for (size_t i = 0; i < doc.GetRowCount(); ++i) {
        std::vector<std::string> row = doc.GetRow<std::string>(i);
        auto skill_opt = skills::SKILLS_DB.get_by_name(row[0]);
        if (!skill_opt) {
            spdlog::error("unable to parse skill: {}", row[0]);
            throw std::runtime_error("unable to parse skill");
        }
        auto skill = *skill_opt;
        int cast_time = (int)(std::stod(std::string{std::string_view{row[1]}.substr(7).substr(
                                  0, row[1].size() - 8)}) *
                              1'000);

        if (i == 0) {
            offset = -cast_time;
        }
        rotation.push_back(skill_cast{tick_t(cast_time + offset), skill});
    }
    return rotation;
}

extern std::vector<skill_cast> read_rotation(const std::string& path) {
    return read_scraped_simple_rotation(path);
}

}  // namespace gw2combat
