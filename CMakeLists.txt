cmake_minimum_required(VERSION 3.2)

include("${CMAKE_SOURCE_DIR}/cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.38.tar.gz"
    SHA1 "ffd013590c9f87b505a350887973af5bd8b68e4c"
)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

project(kgekit)

add_subdirectory(kgekit)
