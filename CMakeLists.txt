cmake_minimum_required(VERSION 3.2)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.27.tar.gz"
    SHA1 "70d75a25f895df3da00d8382417e8c2a08c93173"
)

project(kgekit)

hunter_add_package(pybind11)
find_package(pybind11 CONFIG REQUIRED)

hunter_add_package(Catch)
find_package(Catch2 CONFIG REQUIRED)

set(SOURCES kgekit_impl.cpp)
set(HEADERS kgekit.h)
set(TEST_SOURCES kgekit_impl_test.cpp)

add_library(kgekit_impl ${SOURCES})
target_compile_features(kgekit_impl PUBLIC cxx_std_17)

pybind11_add_module(kgekit SHARED kgekit.cpp)
target_link_libraries(kgekit PRIVATE kgekit_impl pybind11::pybind11 pybind11::embed pybind11::module)

add_executable(kgekit_test ${SOURCES} ${TEST_SOURCES} ${HEADERS})
target_compile_features(kgekit_test PUBLIC cxx_std_17)
target_link_libraries(kgekit_test PUBLIC Catch2::Catch c++experimental)

enable_testing()
add_test(COMMAND kgekit_test)
