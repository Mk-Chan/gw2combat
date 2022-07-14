#ifndef GW2COMBAT_ROTATION_HPP
#define GW2COMBAT_ROTATION_HPP

#include "skills.hpp"

namespace gw2combat {

struct skill_cast {
    tick_t cast_time;
    tick_t cast_duration;
    skills::skill skill;
};

struct predetermined_rotation {
    int offset;
    std::vector<skill_cast> skill_casts;
};

extern predetermined_rotation read_rotation(const std::string& path);

}  // namespace gw2combat

#endif  // GW2COMBAT_ROTATION_HPP
