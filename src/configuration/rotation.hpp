#ifndef GW2COMBAT_CONFIGURATION_ROTATION_HPP
#define GW2COMBAT_CONFIGURATION_ROTATION_HPP

#include "common.hpp"

#include "actor/skill.hpp"

namespace gw2combat::configuration {

struct skill_cast_t {
    actor::skill_t skill;
    tick_t cast_time_ms = 0;
};

struct rotation_t {
    std::vector<skill_cast_t> skill_casts;
    bool repeat = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(skill_cast_t, skill, cast_time_ms)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(rotation_t, skill_casts, repeat)

}  // namespace gw2combat::configuration

#endif  // GW2COMBAT_CONFIGURATION_ROTATION_HPP
