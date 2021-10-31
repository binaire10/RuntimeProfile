cmake_minimum_required(VERSION 3.20)
project(RuntimeProfile)

set(CMAKE_CXX_STANDARD 14)

add_library(Profile)
target_sources(Profile PRIVATE
        src/Profile/SteadyClockProfiler.hpp
        src/Profile/Profile.hpp
        src/Profile/Profile.cpp
        )
target_compile_features(Profile PUBLIC cxx_std_20)
target_include_directories(Profile PUBLIC src)


add_executable(example_profile example/main.cpp)
target_link_libraries(example_profile PRIVATE Profile)
