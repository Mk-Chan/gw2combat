#include "build.hpp"

#include <fstream>

using json = nlohmann::json;

namespace gw2combat {

character_build character_build::read(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    json build_json = json::parse(ifstream);
    return build_json.get<character_build>();
}

}  // namespace gw2combat
