#ifndef GW2COMBAT_COMPONENT_SUCCESSFULLY_CAST_SKILL_HPP
#define GW2COMBAT_COMPONENT_SUCCESSFULLY_CAST_SKILL_HPP

#include <cstdint>

namespace gw2combat::component {

struct successfully_cast_skill {
    enum class skill : std::uint32_t
    {
        NOTHING,
        SKILL_GUARDIAN_GREATSWORD_1_1,
        SKILL_GUARDIAN_GREATSWORD_1_2,
        SKILL_GUARDIAN_GREATSWORD_1_3,
    };
    skill skill_;
};

}  // namespace gw2combat::component

#endif  // GW2COMBAT_COMPONENT_SUCCESSFULLY_CAST_SKILL_HPP
