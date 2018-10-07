hunter_add_package(pybind11)
find_package(pybind11 CONFIG REQUIRED)

hunter_add_package(Catch)
find_package(Catch2 CONFIG REQUIRED)

hunter_add_package(range-v3)
find_package(range-v3 CONFIG REQUIRED)

hunter_add_package(Protobuf)
find_package(Protobuf CONFIG REQUIRED)

add_subdirectory(structs)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        set(FS_LIBRARY c++experimental)
    else()
        set(FS_LIBRARY c++fs)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(FS_LIBRARY stdc++fs)
endif()

set(SOURCES kgekit_impl.cpp indexer.cpp translation.cpp)
set(HEADERS kgekit.h indexer.h translation.h)
set(TEST_SOURCES kgekit_impl_test.cpp indexer_test.cpp translation_test.cpp)

add_library(kgekit_impl ${SOURCES})
set_property(TARGET kgekit_impl PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(kgekit_impl PUBLIC cxx_std_17)
target_link_libraries(kgekit_impl PUBLIC kgekit_structs PRIVATE range-v3::range-v3)

pybind11_add_module(kgekit SHARED kgekit.cpp)
target_link_libraries(kgekit PRIVATE kgekit_impl pybind11::pybind11 pybind11::embed pybind11::module ${FS_LIBRARY})

add_executable(kgekit_test ${SOURCES} ${TEST_SOURCES} ${HEADERS})
target_compile_features(kgekit_test PUBLIC cxx_std_17)
target_link_libraries(kgekit_test PUBLIC Catch2::Catch ${FS_LIBRARY})

enable_testing()
add_test(COMMAND kgekit_test)
