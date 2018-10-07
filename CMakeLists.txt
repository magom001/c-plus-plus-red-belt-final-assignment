cmake_minimum_required(VERSION 3.12)
project(c_plus_plus_red_belt_final_assignment)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(c-plus-plus-testing-framework)
add_subdirectory(c-plus-plus-testing-framework)

include_directories(c-plus-plus-profiler)
add_subdirectory(c-plus-plus-profiler)

add_executable(c_plus_plus_red_belt_final_assignment main.cpp iterator_range.h parse.cpp parse.h search_server.cpp search_server.h heavyTests.h add_duration.h synchronized.h)
target_link_libraries(c_plus_plus_red_belt_final_assignment test_runner profile)
