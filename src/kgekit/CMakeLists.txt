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
set(PY_SOURCES kgekit.cpp entity_number_indexer.cpp)
set(TEST_SOURCES test_helper.cpp kgekit_impl_test.cpp entity_number_indexer_test.cpp entity_number_indexer.h translation_test.cpp translation.h test_helper.h)

add_library(kgekit_impl ${SOURCES})
target_compile_features(kgekit_impl PUBLIC cxx_std_17)
set_property(TARGET kgekit_impl PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(kgekit_impl PUBLIC kgekit_structs range-v3::range-v3)

pybind11_add_module(kgekit SHARED ${PY_SOURCES})
target_compile_features(kgekit PUBLIC cxx_std_17)
target_link_libraries(kgekit PUBLIC kgekit_impl PRIVATE range-v3::range-v3)
set_property(TARGET kgekit PROPERTY PREFIX _)

add_executable(kgekit_test ${TEST_SOURCES} ${PY_SOURCES} ${SOURCES})
target_compile_features(kgekit_test PUBLIC cxx_std_17)
target_link_libraries(kgekit_test PUBLIC Catch2::Catch PRIVATE kgekit_structs pybind11::pybind11 pybind11::embed pybind11::module)

enable_testing()
add_test(COMMAND bin/kgekit_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
