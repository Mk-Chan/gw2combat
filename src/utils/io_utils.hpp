#ifndef GW2COMBAT_UTILS_IO_UTILS_HPP
#define GW2COMBAT_UTILS_IO_UTILS_HPP

#include <fstream>

#include "common.hpp"

namespace gw2combat::utils {

template <typename T>
T read(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    nlohmann::json json = nlohmann::json::parse(ifstream);
    return json.get<T>();
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_IO_UTILS_HPP
