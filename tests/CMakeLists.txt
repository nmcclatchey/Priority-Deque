project(prioritydeque)
cmake_minimum_required(VERSION 3.1)

include(CTest)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-Wall -Wextra)

if (BUILD_TESTING)
    set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
    add_executable(check ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp)
    add_test(NAME standalone_tests COMMAND check)
endif (BUILD_TESTING)