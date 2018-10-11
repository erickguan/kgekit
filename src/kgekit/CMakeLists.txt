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

set(SOURCES kgekit.h kgekit_impl.cpp translation.cpp indexer.h translation.h)
set(PY_SOURCES kgekit.cpp entity_number_indexer.cpp entity_number_indexer.h)
set(TEST_SOURCES kgekit_impl_test.cpp)

add_library(kgekit_impl ${SOURCES})
target_compile_features(kgekit_impl PUBLIC cxx_std_17)
set_property(TARGET kgekit_impl PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(kgekit_impl PUBLIC kgekit_structs PRIVATE range-v3::range-v3)

pybind11_add_module(kgekit SHARED ${PY_SOURCES})
target_compile_features(kgekit PUBLIC cxx_std_17)
target_link_libraries(kgekit PRIVATE kgekit_impl pybind11::pybind11 pybind11::embed pybind11::module range-v3::range-v3 ${FS_LIBRARY})
set_property(TARGET kgekit PROPERTY PREFIX _)

add_executable(kgekit_test ${TEST_SOURCES})
target_compile_features(kgekit_test PUBLIC cxx_std_17)
target_link_libraries(kgekit_test PUBLIC kgekit_structs Catch2::Catch PRIVATE kgekit_impl ${FS_LIBRARY})

enable_testing()
add_test(COMMAND kgekit_test)
