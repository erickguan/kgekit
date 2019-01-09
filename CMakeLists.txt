cmake_minimum_required(VERSION 3.2)

include("${CMAKE_SOURCE_DIR}/cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.44.tar.gz"
    SHA1 "c4cfcc0cd39fdae8de08f6205b7f34cab4a7ba79"
    LOCAL
)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})

project(kgekit)

add_subdirectory(kgedata)
