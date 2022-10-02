cmake_minimum_required(VERSION 3.12)

project(gw2combat)

###
# Find packages
###
find_package(Catch2 3 REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
###

###
# Base directory relative includes for everyone
###
include_directories(src/main)
###

###
# Global Flags
###
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
add_compile_options(-Wall -Wextra -Wpedantic -fpermissive)
###

###
# Flags
###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
###

###
# Tests
###
enable_testing()
add_test(gw2combat_test_build "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target gw2combat_test)
add_test(gw2combat_test_run gw2combat_test)
set_tests_properties(gw2combat_test_run PROPERTIES DEPENDS gw2combat_test_build)
###

###
# Targets
###
add_executable(
    gw2combat

    src/main/gw2combat/main.cpp
    src/main/gw2combat/build_actors.cpp
    src/main/gw2combat/build_skills.cpp
    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp
    src/main/gw2combat/actor/trait_database.cpp
    src/main/gw2combat/actor/unique_effect_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp
    src/main/gw2combat/system/calculate_incoming_damage_modifiers.cpp
    src/main/gw2combat/system/calculate_outgoing_damage_modifiers.cpp
    src/main/gw2combat/system/do_skill.cpp
    src/main/gw2combat/system/dispatch_strikes_and_effects.cpp
    src/main/gw2combat/system/apply_incoming_effects.cpp
    src/main/gw2combat/system/apply_incoming_strikes.cpp
    src/main/gw2combat/system/apply_condition_damage_pulse.cpp
    src/main/gw2combat/system/expire_effects_with_no_duration.cpp

    src/main/gw2combat/system/combat_stats.cpp
    src/main/gw2combat/system/destroy_after_rotation.cpp
    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
    src/main/gw2combat/system/progress_durations.cpp
)
target_link_libraries(gw2combat PRIVATE EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(
    gw2combat_deterministic

    src/main/gw2combat/main.cpp
    src/main/gw2combat/build_actors.cpp
    src/main/gw2combat/build_skills.cpp
    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp
    src/main/gw2combat/actor/trait_database.cpp
    src/main/gw2combat/actor/unique_effect_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp
    src/main/gw2combat/system/calculate_incoming_damage_modifiers.cpp
    src/main/gw2combat/system/calculate_outgoing_damage_modifiers.cpp
    src/main/gw2combat/system/do_skill.cpp
    src/main/gw2combat/system/dispatch_strikes_and_effects.cpp
    src/main/gw2combat/system/apply_incoming_effects.cpp
    src/main/gw2combat/system/apply_incoming_strikes.cpp
    src/main/gw2combat/system/apply_condition_damage_pulse.cpp
    src/main/gw2combat/system/expire_effects_with_no_duration.cpp

    src/main/gw2combat/system/combat_stats.cpp
    src/main/gw2combat/system/destroy_after_rotation.cpp
    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
    src/main/gw2combat/system/progress_durations.cpp
)
target_link_libraries(gw2combat_deterministic PRIVATE EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json spdlog::spdlog)
target_compile_definitions(gw2combat_deterministic PUBLIC DETERMINISTIC)

add_executable(
    gw2combat_test

    src/test/gw2combat/test_build.cpp
    src/test/gw2combat/test_skills.cpp
    src/test/gw2combat/test_rotation.cpp

    src/main/gw2combat/build_actors.cpp
    src/main/gw2combat/build_skills.cpp
    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp
    src/main/gw2combat/actor/trait_database.cpp
    src/main/gw2combat/actor/unique_effect_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp
    src/main/gw2combat/system/calculate_incoming_damage_modifiers.cpp
    src/main/gw2combat/system/calculate_outgoing_damage_modifiers.cpp
    src/main/gw2combat/system/do_skill.cpp
    src/main/gw2combat/system/dispatch_strikes_and_effects.cpp
    src/main/gw2combat/system/apply_incoming_effects.cpp
    src/main/gw2combat/system/apply_incoming_strikes.cpp
    src/main/gw2combat/system/apply_condition_damage_pulse.cpp
    src/main/gw2combat/system/expire_effects_with_no_duration.cpp

    src/main/gw2combat/system/combat_stats.cpp
    src/main/gw2combat/system/destroy_after_rotation.cpp
    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
    src/main/gw2combat/system/progress_durations.cpp
)
target_link_libraries(gw2combat_test PRIVATE Catch2::Catch2WithMain EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json)

configure_file(src/main/resources/skills.json skills.json COPYONLY)
configure_file(src/main/resources/traits.json traits.json COPYONLY)
configure_file(src/main/resources/unique_effects.json unique_effects.json COPYONLY)
configure_file(src/main/resources/build-dh.json build-dh.json COPYONLY)
configure_file(src/main/resources/build-cfb.json build-cfb.json COPYONLY)
configure_file(src/main/resources/build-golem.json build-golem.json COPYONLY)
configure_file(src/main/resources/build-technician.json build-technician.json COPYONLY)
configure_file(src/main/resources/rotation-dh.csv rotation-dh.csv COPYONLY)
configure_file(src/main/resources/rotation-cfb.csv rotation-cfb.csv COPYONLY)
###
