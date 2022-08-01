#ifndef GW2COMBAT_ROTATION_HPP
#define GW2COMBAT_ROTATION_HPP

#include "skills.hpp"

namespace gw2combat {

struct skill_cast {
    tick_t cast_time{0};
    skills::skill skill;
};

extern std::vector<skill_cast> read_rotation(const std::string& path);

}  // namespace gw2combat

#endif  // GW2COMBAT_ROTATION_HPP
