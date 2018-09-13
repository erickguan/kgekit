cmake_minimum_required(VERSION 3.2)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.27.tar.gz"
    SHA1 "70d75a25f895df3da00d8382417e8c2a08c93173"
)

project(kgecpp)

add_library(kgecpp kgekit_impl.cpp)


hunter_add_package(pybind11)
find_package(pybind11 CONFIG REQUIRED)

target_link_libraries(kgecpp pybind11::pybind11 pybind11::embed pybind11::module)
