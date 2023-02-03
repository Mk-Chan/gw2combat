#ifndef GW2COMBAT_COMMON_HPP
#define GW2COMBAT_COMMON_HPP

#include <cstdint>
#include <string>

#include "entt/entt.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include "utils/basic_utils.hpp"
#include "utils/io_utils.hpp"
#include "utils/effect_utils.hpp"

namespace gw2combat {

using tick_t = std::uint32_t;
using entity_t = std::uint32_t;
using registry_t = entt::basic_registry<entity_t>;

}  // namespace gw2combat

#endif  // GW2COMBAT_COMMON_HPP
