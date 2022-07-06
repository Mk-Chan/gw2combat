cmake_minimum_required(VERSION 3.12)

project(gw2combat)

###
# Find packages
###
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
#find_package(Catch2 3 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
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
    src/main/gw2combat/types.hpp
    src/main/gw2combat/entity.hpp src/main/gw2combat/entity.cpp
    src/main/gw2combat/game_loop.hpp src/main/gw2combat/game_loop.cpp

    src/main/gw2combat/component/character_input.hpp
    src/main/gw2combat/component/static_attributes.hpp
    src/main/gw2combat/component/dynamic_attributes.hpp
    src/main/gw2combat/component/boon/might.hpp
    src/main/gw2combat/component/effective_attributes.hpp
    src/main/gw2combat/component/boon/fury.hpp
    src/main/gw2combat/component/animation.hpp
    src/main/gw2combat/component/targeting.hpp
    src/main/gw2combat/component/combat_stats.hpp
    src/main/gw2combat/component/outgoing_damage.hpp
    src/main/gw2combat/component/incoming_damage.hpp
    src/main/gw2combat/component/effective_incoming_damage.hpp
    src/main/gw2combat/component/downstate.hpp
    src/main/gw2combat/component/successfully_cast_skill.hpp
    src/main/gw2combat/component/boon/quickness.hpp
    src/main/gw2combat/component/boon/resolution.hpp
    src/main/gw2combat/component/boon/aegis.hpp
    src/main/gw2combat/component/condition/vulnerability.hpp
    src/main/gw2combat/component/condition/burning.hpp

    src/main/gw2combat/system/system.hpp
    src/main/gw2combat/system/character_command.cpp
    src/main/gw2combat/system/effective_attributes_calculation.cpp
    src/main/gw2combat/system/character_animation.cpp
    src/main/gw2combat/system/effect_expiration.cpp
    src/main/gw2combat/system/outgoing_strike_damage_calculation.cpp
    src/main/gw2combat/system/combat_detection.cpp
    src/main/gw2combat/system/incoming_strike_damage_detection.cpp
    src/main/gw2combat/system/incoming_strike_damage_calculation.cpp
    src/main/gw2combat/system/update_combat_stats.cpp
    src/main/gw2combat/system/downstate_detection.cpp
    src/main/gw2combat/system/accumulate_skill_cast_ticks.cpp
    src/main/gw2combat/iterable_priority_queue.hpp src/main/gw2combat/effect.hpp src/main/gw2combat/component/condition_tick_status.hpp src/main/gw2combat/system/process_condition_tick.cpp src/main/gw2combat/system/incoming_condition_damage_calculation.cpp src/main/gw2combat/system/outgoing_condition_application.cpp src/main/gw2combat/component/profession/guardian/virtue_of_justice.hpp)
target_link_libraries(gw2combat PRIVATE fmt::fmt spdlog::spdlog nlohmann_json::nlohmann_json EnTT::EnTT)

# gw2combat_test
#add_executable(
#    gw2combat_test
#    src/test/gw2combat/example.cpp
#    src/main/gw2combat/process/player_character.cpp
#    src/main/gw2combat/process/player_character.h
#    src/main/gw2combat/scheduler.h
#    src/main/gw2combat/context.h
#    src/main/gw2combat/context.cpp
#    src/main/gw2combat/component/player_command.h
#    src/main/gw2combat/entities.h src/main/gw2combat/entities.cpp src/main/gw2combat/component/position.h src/main/gw2combat/component/character_static_attributes.h src/main/gw2combat/component/character_dynamic_attributes.h)
#target_link_libraries(gw2combat_test PRIVATE Catch2::Catch2WithMain fmt::fmt EnTT::EnTT)
###
