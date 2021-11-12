cmake_minimum_required(VERSION 3.20)
project(RuntimeProfile)

find_package(Boost COMPONENTS date_time system)
find_package(Qt5 COMPONENTS Widgets Network)

option(RuntimeProfile_TOOLS OFF)

add_library(Profile)
target_sources(Profile PRIVATE
        src/Profile/SteadyClockProfiler.hpp
        src/Profile/Profile.hpp
        src/Profile/Profile.cpp
        )
target_compile_features(Profile PUBLIC cxx_std_17)
target_include_directories(Profile PUBLIC src)
if (Boost_FOUND)
    target_compile_definitions(Profile PUBLIC HAS_BOOST)
    target_link_libraries(Profile PUBLIC Boost::date_time Boost::system $<${WIN32}:ws2_32>)
endif ()


add_executable(example_profile example/main.cpp)
target_link_libraries(example_profile PRIVATE Profile)

if (Boost_FOUND)
    add_executable(profile_capture example/profile_capture.cpp)
    target_link_libraries(profile_capture PRIVATE Profile)
endif ()

if (RuntimeProfile_TOOLS AND Qt5_FOUND)
    add_executable(profile_capture_gui tools/profile_capture_gui.cpp)
    target_link_libraries(profile_capture_gui PRIVATE Profile Qt5::Network Qt5::Widgets)
endif ()