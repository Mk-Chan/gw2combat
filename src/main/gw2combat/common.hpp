#ifndef GW2COMBAT_TYPES_HPP
#define GW2COMBAT_TYPES_HPP

#include <cstdint>
#include <string>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

namespace gw2combat {

#if DETERMINISTIC
constexpr static bool DETERMINISTIC_SIMULATION = true;
#else
constexpr static bool DETERMINISTIC_SIMULATION = false;
#endif

using tick_t = std::uint32_t;
using entity_t = std::uint32_t;
using registry_t = entt::basic_registry<entity_t>;

}  // namespace gw2combat

#endif  // GW2COMBAT_TYPES_HPP
