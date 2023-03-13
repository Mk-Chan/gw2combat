#ifndef GW2COMBAT_COMMON_HPP
#define GW2COMBAT_COMMON_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

#include "entt/entt.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

namespace gw2combat {

using tick_t = std::uint32_t;
using entity_t = std::uint32_t;
using registry_t = entt::basic_registry<entity_t>;

}  // namespace gw2combat

#endif  // GW2COMBAT_COMMON_HPP
