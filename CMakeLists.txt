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
add_compile_options(-Wall -Wextra -Wpedantic)
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
    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp
    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
)
target_link_libraries(gw2combat PRIVATE EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json spdlog::spdlog)

add_executable(
    gw2combat_deterministic

    src/main/gw2combat/main.cpp
    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp
    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
)
target_link_libraries(gw2combat_deterministic PRIVATE EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json spdlog::spdlog)
target_compile_definitions(gw2combat_deterministic PUBLIC DETERMINISTIC)

add_executable(
    gw2combat_test

    src/test/gw2combat/test_build.cpp
    src/test/gw2combat/test_skills.cpp
    src/test/gw2combat/test_rotation.cpp

    src/main/gw2combat/combat_loop.cpp

    src/main/gw2combat/actor/build.cpp
    src/main/gw2combat/actor/rotation.cpp
    src/main/gw2combat/actor/skill_database.cpp

    src/main/gw2combat/system/calculate_effective_attributes.cpp

    src/main/gw2combat/system/continue_rotation.cpp
    src/main/gw2combat/system/do_animation.cpp
    src/main/gw2combat/system/progress_cooldowns.cpp
    src/main/gw2combat/system/progress_recharges.cpp
)
target_link_libraries(gw2combat_test PRIVATE Catch2::Catch2WithMain EnTT::EnTT fmt::fmt nlohmann_json::nlohmann_json)


FILE(
    COPY
    src/main/resources/skills.json
    src/main/resources/build-cfb.json
    src/main/resources/build-dh.json
    src/main/resources/build-technician.json
    src/main/resources/build-golem.json

    src/main/resources/rotation-cfb.csv
    src/main/resources/rotation-dh.csv

    DESTINATION
    "${CMAKE_BINARY_DIR}"
)
###
