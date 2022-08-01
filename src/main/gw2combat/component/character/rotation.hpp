#ifndef GW2COMBAT_SRC_MAIN_GW2COMBAT_COMPONENT_CHARACTER_ROTATION_HPP
#define GW2COMBAT_SRC_MAIN_GW2COMBAT_COMPONENT_CHARACTER_ROTATION_HPP

#include "gw2combat/rotation.hpp"

namespace gw2combat::component {

struct rotation {
    std::vector<skill_cast> skill_casts;
    size_t current_idx{0};
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_SRC_MAIN_GW2COMBAT_COMPONENT_CHARACTER_ROTATION_HPP
