cmake_minimum_required(VERSION 3.1)
project(porter)

option(PORTER_BUILD_TESTS "Build Tests" OFF)

add_subdirectory(deps)
add_subdirectory(src)

if (PORTER_BUILD_TESTS)
    add_subdirectory(test)
endif (PORTER_BUILD_TESTS)
