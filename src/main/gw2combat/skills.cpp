#include "skills.hpp"

namespace gw2combat::skills {

constexpr static inline skill GUARDIAN_GREATSWORD_1_1{.name = "Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {600, 400},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = {0, 0},
                                                      .damage_coefficient = 0.8};
constexpr static inline skill GUARDIAN_GREATSWORD_1_2{.name = "Vengeful Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {840, 600},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = {0, 0},
                                                      .damage_coefficient = 0.8};
constexpr static inline skill GUARDIAN_GREATSWORD_1_3{.name = "Wrathful Strike"_hs,
                                                      .type = skill::type::CASTING_NO_AFTER_CAST,
                                                      .cast_duration = {1000, 675},
                                                      .damage_start_after_cast_end = 0,
                                                      .damage_duration = 0,
                                                      .hits = 1,
                                                      .cooldown = {0, 0},
                                                      .damage_coefficient = 1.2};
constexpr static inline skill GUARDIAN_GREATSWORD_2{.name = "Whirling Wrath"_hs,
                                                    .type = skill::type::CHANNELING_NO_AFTER_CAST,
                                                    .cast_duration = {2000, 1475},
                                                    .hits = 14,
                                                    .cooldown = {8000, 6400},
                                                    .damage_coefficient = 0.4125};
constexpr static inline skill GUARDIAN_GREATSWORD_3{.name = "Leap of Faith"_hs,
                                                    .type = skill::type::CASTING_NO_AFTER_CAST,
                                                    .cast_duration = {1000, 720},  // confirm
                                                    .hits = 1,
                                                    .cooldown = {12000, 9600},
                                                    .damage_coefficient = 1.25};
constexpr static inline skill GUARDIAN_GREATSWORD_5{.name = "Binding Blade"_hs,
                                                    .type = skill::type::CASTING_WITH_AFTER_CAST,
                                                    .cast_duration = {720, 475},  // confirm
                                                    .hits = 1,
                                                    .cooldown = {20000, 16000},
                                                    .damage_coefficient = 1.0};
constexpr static inline skill GUARDIAN_FOCUS_5{.name = "Shield of Wrath"_hs,
                                               .type = skill::type::INSTANT_CAST,
                                               .cast_duration = {0, 0},
                                               .damage_start_after_cast_end = 4000,
                                               .hits = 1,
                                               .cooldown = {35000, 28000},
                                               .damage_coefficient = 2.5};

std::vector<skill> ALL_SKILLS{GUARDIAN_GREATSWORD_1_1,
                              GUARDIAN_GREATSWORD_1_2,
                              GUARDIAN_GREATSWORD_1_3,
                              GUARDIAN_GREATSWORD_2,
                              GUARDIAN_GREATSWORD_3,
                              GUARDIAN_GREATSWORD_5,
                              GUARDIAN_FOCUS_5};

}  // namespace gw2combat::skills
