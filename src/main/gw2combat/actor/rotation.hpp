#ifndef GW2COMBAT_ACTOR_ROTATION_HPP
#define GW2COMBAT_ACTOR_ROTATION_HPP

#include "gw2combat/common.hpp"

#include "skill.hpp"

namespace gw2combat::actor {

struct skill_cast_t {
    skill_t skill;
    tick_t cast_time_ms;
};

struct rotation_t {
    [[nodiscard]] static rotation_t read(const std::string& path, base_class_t base_class);

    std::vector<skill_cast_t> skill_casts;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_t, skill, cast_time_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(rotation_t, skill_casts)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_ROTATION_HPP
