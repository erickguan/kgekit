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

hunter_add_package(range-v3)
find_package(range-v3 CONFIG REQUIRED)

hunter_add_package(Protobuf)
find_package(Protobuf CONFIG REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(structs)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(FS_LIBRARY c++experimental)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(FS_LIBRARY stdc++fs)
endif()

set(SOURCES kgekit_impl.cpp indexer.cpp translator.cpp)
set(HEADERS kgekit.h indexer.h translator.h)
set(TEST_SOURCES kgekit_impl_test.cpp indexer_test.cpp translator_test.cpp)

add_library(kgekit_impl ${SOURCES})
target_compile_features(kgekit_impl PUBLIC cxx_std_17)
target_link_libraries(kgekit_impl PUBLIC kgekit_structs PRIVATE range-v3::range-v3)

pybind11_add_module(kgekit SHARED kgekit.cpp)
target_link_libraries(kgekit PRIVATE kgekit_impl pybind11::pybind11 pybind11::embed pybind11::module ${FS_LIBRARY})

add_executable(kgekit_test ${SOURCES} ${TEST_SOURCES} ${HEADERS})
target_compile_features(kgekit_test PUBLIC cxx_std_17)
target_link_libraries(kgekit_test PUBLIC Catch2::Catch ${FS_LIBRARY})

enable_testing()
add_test(COMMAND kgekit_test)
