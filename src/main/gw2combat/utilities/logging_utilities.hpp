#ifndef GW2COMBAT_UTILITIES_LOGGING_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_LOGGING_UTILITIES_HPP

#include "base_utilities.hpp"

#include <string>
#include <string_view>

#include "gw2combat/component/actor/animation.hpp"
#include "gw2combat/component/actor/effective_attributes.hpp"

namespace gw2combat::utils {

template <typename T>
constexpr std::string_view get_component_name() {
    std::string_view name = entt::type_id<T>().name();
    name.remove_prefix(strlen("gw2combat::component::"));
    return name;
}

template <typename T>
static inline void log_component(registry_t& registry) {
    registry.template view<T>().each([&](entity_t entity, const T& t) {
        spdlog::info("[{}] {}:{} - {}",
                     get_current_tick(registry),
                     get_entity_name(entity, registry),
                     get_component_name<T>(),
                     to_string(t));
    });
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_LOGGING_UTILITIES_HPP
