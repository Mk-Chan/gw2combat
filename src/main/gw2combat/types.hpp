#ifndef GW2COMBAT_TYPES_HPP
#define GW2COMBAT_TYPES_HPP

#include <cstdint>
#include <string>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

namespace gw2combat {

using namespace entt::literals;

using tick_t = std::uint32_t;
using entity_t = entt::entity;
using registry_t = entt::registry;

enum class combat_stage : std::uint32_t
{
    UNDEFINED,
    BEFORE_OUTGOING_STRIKE_BUFFERING,
    AFTER_OUTGOING_STRIKE_BUFFERING,
    BEFORE_INCOMING_STRIKE_DAMAGE_CALCULATION,
    AFTER_INCOMING_STRIKE_DAMAGE_CALCULATION,
};

[[nodiscard]] static inline std::uint32_t to_u32(entity_t entity) {
    return static_cast<std::uint32_t>(entity);
}

}  // namespace gw2combat

#endif  // GW2COMBAT_TYPES_HPP
