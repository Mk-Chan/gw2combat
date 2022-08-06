#ifndef GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
#define GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP

#include "base_utilities.hpp"

#include "gw2combat/skills.hpp"

namespace gw2combat::utils {

[[nodiscard]] static inline bool skill_has_tag(const skills::skill& skill,
                                               skills::skill_tag skill_tag) {
    return std::find(skill.tags.cbegin(), skill.tags.cend(), skill_tag) != skill.tags.cend();
}

}  // namespace gw2combat::utils

#endif  // GW2COMBAT_UTILITIES_SKILL_UTILITIES_HPP
