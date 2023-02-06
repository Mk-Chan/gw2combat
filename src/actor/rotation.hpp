#ifndef GW2COMBAT_ACTOR_ROTATION_HPP
#define GW2COMBAT_ACTOR_ROTATION_HPP

#include "common.hpp"

#include "skill.hpp"

namespace gw2combat::actor {

struct skill_cast_t {
    skill_t skill;
    tick_t cast_time_ms = 0;
};

struct rotation_t {
    std::vector<skill_cast_t> skill_casts;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_t, skill, cast_time_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(rotation_t, skill_casts)

}  // namespace gw2combat::actor

#endif  // GW2COMBAT_ACTOR_ROTATION_HPP
