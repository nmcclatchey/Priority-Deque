project(prioritydeque)
cmake_minimum_required(VERSION 3.1)

include(CTest)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-Wall -Wextra)

if (BUILD_TESTING)
  find_package(Boost)
  if (Boost_FOUND)
    add_executable(check ${CMAKE_CURRENT_SOURCE_DIR}/tests/boost_test_main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_interval_heap.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_priority_deque.cpp)
    target_include_directories(check PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})
    target_link_libraries(check PUBLIC ${Boost_LIBRARIES})
    add_test(NAME boost_tests COMMAND check)
  else (Boost_FOUND)
    add_executable(check tests/tests.cpp)
    add_test(NAME standalone_tests COMMAND check)
  endif (Boost_FOUND)
endif (BUILD_TESTING)