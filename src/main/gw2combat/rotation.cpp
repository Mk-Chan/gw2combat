#include "rotation.hpp"

#include <spdlog/spdlog.h>
#include <filesystem>

#include "rapidcsv.h"

namespace gw2combat {

predetermined_rotation read_scraped_simple_rotation(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    const auto file_size = std::filesystem::file_size(file_path);

    std::string csv(file_size, '\0');

    ifstream.read(csv.data(), file_size);

    std::stringstream csv_stream{csv};
    rapidcsv::Document doc{csv_stream};

    predetermined_rotation predetermined_rotation;
    int cast_duration_accumulated_error = 0;
    for (size_t i = 0; i < doc.GetRowCount(); ++i) {
        std::vector<std::string> row = doc.GetRow<std::string>(i);
        skills::skill skill = skills::get_by_name(entt::hashed_string{row[0].c_str()});
        int cast_time = (int)(std::stod(std::string{std::string_view{row[1]}.substr(7).substr(
                                  0, row[1].size() - 8)}) *
                              1'000);
        int cast_duration_in_csv =
            std::stoi(std::string{std::string_view{row[2]}.substr(6).substr(0, row[2].size() - 8)});

        if (i == 0) {
            predetermined_rotation.offset = -cast_time;
        }
        predetermined_rotation.skill_casts.push_back(
            skill_cast{tick_t(cast_time + predetermined_rotation.offset),
                       tick_t(cast_duration_in_csv),
                       skill});

        cast_duration_accumulated_error += cast_duration_in_csv - (int)skill.cast_duration[1];
    }
    spdlog::info("cast_duration_error: {}", cast_duration_accumulated_error);

    return predetermined_rotation;
}

extern predetermined_rotation read_rotation(const std::string& path) {
    return read_scraped_simple_rotation(path);
}

}  // namespace gw2combat
