#ifndef GW2COMBAT_SKILL_HPP
#define GW2COMBAT_SKILL_HPP

#include "fmt/format.h"

#include "entt/entt.hpp"

#include "types.hpp"

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
    unsigned int damage_start_after_cast_end;   // maybe array of size 2 for quickness?
    unsigned int damage_duration;
    unsigned int hits;
    unsigned int cooldown;
    double damage_coefficient;
};

constexpr static inline skill IDLE{.name = "Idle"_hs,
                                   .type = skill::type::CASTING_NO_AFTER_CAST,
                                   .cast_duration = {1000, 1000},
                                   .damage_start_after_cast_end = 0,
                                   .damage_duration = 0,
                                   .hits = 0,
                                   .damage_coefficient = 0.0};
constexpr static inline skill GUARDIAN_GREATSWORD_1_1{.name = "Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {600, 400},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = 0,
                                                      .damage_coefficient = 0.8};
constexpr static inline skill GUARDIAN_GREATSWORD_1_2{.name = "Vengeful Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {840, 600},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = 0,
                                                      .damage_coefficient = 0.8};
constexpr static inline skill GUARDIAN_GREATSWORD_1_3{.name = "Wrathful Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {1000, 675},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = 0,
                                                      .damage_coefficient = 1.2};
constexpr static inline skill GUARDIAN_GREATSWORD_2{.name = "Whirling Wrath"_hs,
                                                    .type = skill::type::CHANNELING_NO_AFTER_CAST,
                                                    .cast_duration = {2000, 1475},
                                                    .hits = 14,
                                                    .cooldown = 8000,
                                                    .damage_coefficient = 0.4125};
constexpr static inline skill GUARDIAN_FOCUS_5{.name = "Shield of Wrath"_hs,
                                               .type = skill::type::INSTANT_CAST,
                                               .cast_duration = {0, 0},
                                               .damage_start_after_cast_end = 4000,
                                               .hits = 1,
                                               .cooldown = 8000,
                                               .damage_coefficient = 2.5};

constexpr static inline std::array ALL_SKILLS{GUARDIAN_GREATSWORD_1_1,
                                              GUARDIAN_GREATSWORD_1_2,
                                              GUARDIAN_GREATSWORD_1_3,
                                              GUARDIAN_GREATSWORD_2,
                                              GUARDIAN_FOCUS_5};

constexpr static inline skills::skill get_by_name(const entt::hashed_string name) {
    for (auto& skill : ALL_SKILLS) {
        if (name == skill.name) {
            return skill;
        }
    }
    throw std::invalid_argument(fmt::format("skill name {} not found!", name.data()));
}

}  // namespace gw2combat::skills

#endif  // GW2COMBAT_SKILL_HPP
