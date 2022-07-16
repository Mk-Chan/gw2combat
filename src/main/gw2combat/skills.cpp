#include "skills.hpp"

namespace gw2combat::skills {

static inline skill GUARDIAN_GREATSWORD_1_1{.name = "Strike"_hs,
                                            .type = skill::type::CASTING_NO_AFTER_CAST,
                                            .cast_duration = {600, 400},
                                            .damage_start_after_cast_end = 0,
                                            .damage_duration = 0,
                                            .hits = 1,
                                            .cooldown = {0, 0},
                                            .damage_coefficient = 0.8,
                                            .weapon = GREATSWORD};
static inline skill GUARDIAN_GREATSWORD_1_2{.name = "Vengeful Strike"_hs,
                                            .type = skill::type::CASTING_NO_AFTER_CAST,
                                            .cast_duration = {840, 600},
                                            .damage_start_after_cast_end = 0,
                                            .damage_duration = 0,
                                            .hits = 1,
                                            .cooldown = {0, 0},
                                            .damage_coefficient = 0.8,
                                            .weapon = GREATSWORD};
static inline skill GUARDIAN_GREATSWORD_1_3{.name = "Wrathful Strike"_hs,
                                            .type = skill::type::CASTING_NO_AFTER_CAST,
                                            .cast_duration = {1'000, 675},
                                            .damage_start_after_cast_end = 0,
                                            .damage_duration = 0,
                                            .hits = 1,
                                            .cooldown = {0, 0},
                                            .damage_coefficient = 1.2,
                                            .weapon = GREATSWORD};
static inline skill GUARDIAN_GREATSWORD_2{.name = "Whirling Wrath"_hs,
                                          .type = skill::type::CHANNELING_NO_AFTER_CAST,
                                          .cast_duration = {2'000, 1'475},
                                          .hits = 14,
                                          .cooldown = {8'000, 6'400},
                                          .damage_coefficient = 0.4125,
                                          .weapon = GREATSWORD};
static inline skill GUARDIAN_GREATSWORD_3{.name = "Leap of Faith"_hs,
                                          .type = skill::type::CASTING_NO_AFTER_CAST,
                                          .cast_duration = {1000, 720},  // confirm
                                          .hits = 1,
                                          .cooldown = {12'000, 9'600},
                                          .damage_coefficient = 1.25,
                                          .weapon = GREATSWORD};
static inline skill GUARDIAN_GREATSWORD_5{.name = "Binding Blade"_hs,
                                          .type = skill::type::CASTING_WITH_AFTER_CAST,
                                          .cast_duration = {720, 475},  // confirm
                                          .hits = 1,
                                          .cooldown = {20'000, 16'000},
                                          .damage_coefficient = 1.0,
                                          .weapon = GREATSWORD};
static inline skill GUARDIAN_FOCUS_5{.name = "Shield of Wrath"_hs,
                                     .type = skill::type::INSTANT_CAST,
                                     .cast_duration = {0, 0},
                                     .damage_start_after_cast_end = 4000,
                                     .hits = 1,
                                     .cooldown = {35'000, 28'000},
                                     .damage_coefficient = 2.5};

static inline skill SWORD_OF_JUSTICE{.name = "Sword of Justice"_hs,
                                     .type = skill::type::CASTING_WITH_AFTER_CAST,
                                     .cast_duration = {900, 600},
                                     .cooldown = {20'000, 13'333}};
static inline skill SWORD_OF_JUSTICE_ATTACK{.name = "Sword of Justice Attack"_hs,
                                            .type = skill::type::CHANNELING_NO_AFTER_CAST,
                                            .cast_duration = {1600, 1600},
                                            .hits = 4,
                                            .damage_coefficient = 0.72};

std::vector<skill> ALL_SKILLS{GUARDIAN_GREATSWORD_1_1,
                              GUARDIAN_GREATSWORD_1_2,
                              GUARDIAN_GREATSWORD_1_3,
                              GUARDIAN_GREATSWORD_2,
                              GUARDIAN_GREATSWORD_3,
                              GUARDIAN_GREATSWORD_5,
                              GUARDIAN_FOCUS_5,

                              SWORD_OF_JUSTICE,
                              SWORD_OF_JUSTICE_ATTACK};

}  // namespace gw2combat::skills
