#ifndef GW2COMBAT_UTILS_IO_UTILS_HPP
#define GW2COMBAT_UTILS_IO_UTILS_HPP

#include <fstream>

#include "common.hpp"

#include "entity_utils.hpp"

namespace gw2combat::utils {

template <typename T>
static inline std::string_view get_component_name_with_prefix() {
    std::string_view name = entt::type_id<T>().name();
    return name;
}

template <typename T>
static inline std::string_view get_component_name() {
    std::string_view name = entt::type_id<T>().name();
    name.remove_prefix(strlen("gw2combat::component::"));
    return name;
}

template <typename T>
static inline void log_component(registry_t& registry) {
    registry.view<T>().each([&](entity_t entity, const T& t) {
        spdlog::info("[{}] {}:{} - {}",
                     get_current_tick(registry),
                     get_entity_name(entity, registry),
                     get_component_name<T>(),
                     to_string(t));
    });
}

template <typename T>
static inline T read(const std::string& path) {
    const auto file_path = std::filesystem::path(path);
    std::ifstream ifstream{file_path, std::ios::in};
    nlohmann::json json = nlohmann::json::parse(ifstream);
    return json.get<T>();
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILS_IO_UTILS_HPP
