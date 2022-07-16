#ifndef GW2COMBAT_SKILL_HPP
#define GW2COMBAT_SKILL_HPP

#include "fmt/format.h"

#include "entt/entt.hpp"

#include "types.hpp"
#include "weapon.hpp"

using namespace entt::literals;

namespace gw2combat::skills {

struct skill {
    enum class type : std::uint32_t
    {
        INSTANT_CAST,
        CASTING_NO_AFTER_CAST,
        CASTING_WITH_AFTER_CAST,
        CHANNELING_NO_AFTER_CAST,
        CHANNELING_WITH_AFTER_CAST,
    };

    constexpr inline bool operator==(const skill& rhs) const {
        return this->name == rhs.name;
    }

    entt::hashed_string name;
    type type;
    std::array<unsigned int, 2> cast_duration;  // no-quickness = 0, quickness = 1
    unsigned int damage_start;                  // maybe array of size 2 for quickness?
    unsigned int hits;
    std::array<unsigned int, 2> cooldown;  // no-alacrity = 0, alacrity = 1
    double damage_coefficient;
    weapon weapon = EMPTY_HANDED;
};

static inline skill IDLE{.name = "Idle"_hs,
                         .type = skill::type::CASTING_NO_AFTER_CAST,
                         .cast_duration = {1000, 1000},
                         .damage_start = 0,
                         .hits = 0,
                         .damage_coefficient = 0.0};

extern std::vector<skill> ALL_SKILLS;

static inline skills::skill get_by_name(const entt::hashed_string name) {
    for (auto& skill : ALL_SKILLS) {
        if (name == skill.name) {
            return skill;
        }
    }
    throw std::invalid_argument(fmt::format("skill name {} not found!", name.data()));
}

}  // namespace gw2combat::skills

#endif  // GW2COMBAT_SKILL_HPP
