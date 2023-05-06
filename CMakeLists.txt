cmake_minimum_required(VERSION 3.12)

project(gw2combat)

###
# Base directory relative includes for everyone
###
include_directories(src include)
###

###
# Global Flags
###
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
add_compile_options(-Wall -Wextra -Wpedantic -Wno-deprecated)
###

###
# Flags
###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g3 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lwsock32 -lws2_32 -m64 -Wa,-mbig-obj")
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
file(GLOB gw2combat_src CONFIGURE_DEPENDS "src/*.cpp" "src/*/*.cpp" "src/*.hpp" "src/*/*.hpp")
add_executable(gw2combat ${gw2combat_src})
if (NOT APPLE)
    target_link_libraries(gw2combat "-static")
endif ()
###
