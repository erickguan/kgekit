cmake_minimum_required(VERSION 3.2)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.31.tar.gz"
    SHA1 "3d94b8522541c4c8eaac606f4c47d09322a8019c"
)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

project(kgekit)

add_subdirectory(kgekit)
