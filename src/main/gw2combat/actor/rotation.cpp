#include "rotation.hpp"

#include <fstream>

namespace gw2combat::actor {

rotation_t rotation_t::read(const std::string& path, base_class_t base_class) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};

    rotation_t rotation;
    int cast_time_offset_ms;
    int line_num = -1;
    for (std::string line; std::getline(ifstream, line); ++line_num) {
        if (line_num == -1) {
            continue;
        }

        auto delimiter_pos = line.find(',');
        if (delimiter_pos == std::string::npos) {
            throw std::runtime_error(
                fmt::format("unable to read rotation. line_num: {}", line_num));
        }

        skill_t skill{base_class, line.substr(0, delimiter_pos)};
        std::string time_str = line.substr(delimiter_pos + 2);

        auto last_delimiter_pos = time_str.find(',');
        if (last_delimiter_pos == std::string::npos) {
            last_delimiter_pos = time_str.length();
        }
        last_delimiter_pos -= 6;

        int cast_time_ms = (int)(std::stod(time_str.substr(6, last_delimiter_pos - 1)) * 1'000);
        if (line_num == 0) {
            cast_time_offset_ms = -cast_time_ms;
        }
        rotation.skill_casts.emplace_back(
            skill_cast_t{skill, (tick_t)(cast_time_ms + cast_time_offset_ms)});
    }

    if (line_num == -1) {
        spdlog::warn("could not read file {}", path);
    }

    return rotation;
}

}  // namespace gw2combat::actor
