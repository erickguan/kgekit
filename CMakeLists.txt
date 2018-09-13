cmake_minimum_required(VERSION 3.2)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.27.tar.gz"
    SHA1 "70d75a25f895df3da00d8382417e8c2a08c93173"
)

project(kgekit)

hunter_add_package(pybind11)
find_package(pybind11 CONFIG REQUIRED)

add_library(kgekit_impl kgekit_impl.cpp)
pybind11_add_module(kgekit SHARED kgekit.cpp)
target_link_libraries(kgekit PRIVATE kgekit_impl pybind11::pybind11 pybind11::embed pybind11::module)
