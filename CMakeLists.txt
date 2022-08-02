cmake_minimum_required(VERSION 3.12)

project(gw2combat)

###
# Find packages
###
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
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
###

###
# Tests
###
#enable_testing()
#add_test(gw2combat_test_build "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target gw2combat_test)
#add_test(gw2combat_test_run gw2combat_test)
#set_tests_properties(gw2combat_test_run PROPERTIES DEPENDS gw2combat_test_build)
###

###
# Targets
###
# gw2combat
add_executable(
    gw2combat

    src/main/gw2combat/main.cpp
    src/main/gw2combat/game_loop.cpp

    src/main/gw2combat/entity.cpp
    src/main/gw2combat/skills.cpp
    src/main/gw2combat/rotation.cpp

    src/main/gw2combat/system/system.cpp
    src/main/gw2combat/system/character_command.cpp
    src/main/gw2combat/system/effective_attributes_calculation.cpp
    src/main/gw2combat/system/expire_non_damaging_effects.cpp
    src/main/gw2combat/system/combat_detection.cpp
    src/main/gw2combat/system/update_health.cpp
    src/main/gw2combat/system/downstate_detection.cpp
    src/main/gw2combat/system/damage/pulse_conditions.cpp

    src/main/gw2combat/system/damage/detection/incoming_strike_detection.cpp
    src/main/gw2combat/system/damage/detection/incoming_condition_detection.cpp
    src/main/gw2combat/system/damage/detection/incoming_condition_application.cpp

    src/main/gw2combat/system/damage/multipliers/incoming_strike_damage_multiplier_calculation.cpp
    src/main/gw2combat/system/damage/multipliers/incoming_condition_damage_multiplier_calculation.cpp
    src/main/gw2combat/system/damage/multipliers/outgoing_condition_damage_multiplier_calculation.cpp
    src/main/gw2combat/system/damage/multipliers/outgoing_strike_damage_multiplier_calculation.cpp

    src/main/gw2combat/system/damage/incoming_strike_damage_calculation.cpp
    src/main/gw2combat/system/damage/expire_damaging_effects.cpp

    src/main/gw2combat/system/cast_skills.cpp
    src/main/gw2combat/system/staged/trait_systems.hpp
    src/main/gw2combat/system/destroy_after_rotation_entities.cpp
    src/main/gw2combat/system/staged/skill_systems.hpp
    src/main/gw2combat/component/trait_components.hpp)
target_link_libraries(gw2combat PRIVATE fmt::fmt spdlog::spdlog nlohmann_json::nlohmann_json EnTT::EnTT)
target_include_directories(gw2combat PRIVATE ${RAPIDCSV_INCLUDE_DIRS})
###
