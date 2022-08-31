#include "build.hpp"

#include <fstream>

namespace gw2combat::actor {

build_t build_t::read(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    nlohmann::json build_json = nlohmann::json::parse(ifstream);
    return build_json.get<build_t>();
}

}  // namespace gw2combat::actor
