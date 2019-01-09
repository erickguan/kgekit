hunter_add_package(pybind11)
find_package(pybind11 CONFIG REQUIRED)

hunter_add_package(Catch)
find_package(Catch2 CONFIG REQUIRED)

hunter_add_package(Boost)
find_package(Boost CONFIG REQUIRED)

hunter_add_package(Protobuf)
find_package(Protobuf CONFIG REQUIRED)

hunter_add_package(fmt)
find_package(fmt CONFIG REQUIRED)

add_subdirectory(messages)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        set(FS_LIBRARY c++experimental)
    else()
        set(FS_LIBRARY c++fs)
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(FS_LIBRARY stdc++fs)
endif()

set(SOURCES kgedata.h kgedata.cpp)
set(PY_SOURCES kgedata_py.cpp entity_number_indexer.cpp lcwa_no_throw_sampler.cpp corruptors.cpp ranker.cpp triple_expander.cpp)
set(TEST_SOURCES test_helper.cpp kgedata_test.cpp entity_number_indexer_test.cpp entity_number_indexer.h test_helper.h)

add_library(kgedata_impl ${SOURCES})
target_compile_features(kgedata_impl PUBLIC cxx_std_17)
set_property(TARGET kgedata_impl PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(kgedata_impl PUBLIC kgedata_structs fmt::fmt)

pybind11_add_module(kgedata SHARED ${PY_SOURCES})
target_compile_features(kgedata PUBLIC cxx_std_17)
target_link_libraries(kgedata PUBLIC kgedata_impl PRIVATE Boost::boost)

# test target and helper, from pybind11 tools

# Checks whether the given CXX/linker flags can compile and link a cxx file.  cxxflags and
# linkerflags are lists of flags to use.  The result variable is a unique variable name for each set
# of flags: the compilation result will be cached base on the result variable.  If the flags work,
# sets them in cxxflags_out/linkerflags_out internal cache variables (in addition to ${result}).
function(_pybind11_return_if_cxx_and_linker_flags_work result cxxflags linkerflags cxxflags_out linkerflags_out)
    set(CMAKE_REQUIRED_LIBRARIES ${linkerflags})
    check_cxx_compiler_flag("${cxxflags}" ${result})
    if (${result})
        set(${cxxflags_out} "${cxxflags}" CACHE INTERNAL "" FORCE)
        set(${linkerflags_out} "${linkerflags}" CACHE INTERNAL "" FORCE)
    endif()
endfunction()

# Internal: find the appropriate link time optimization flags for this compiler
function(_pybind11_add_lto_flags target_name prefer_thin_lto)
    if (NOT DEFINED PYBIND11_LTO_CXX_FLAGS)
        set(PYBIND11_LTO_CXX_FLAGS "" CACHE INTERNAL "")
        set(PYBIND11_LTO_LINKER_FLAGS "" CACHE INTERNAL "")

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            set(cxx_append "")
            set(linker_append "")
            if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT APPLE)
                # Clang Gold plugin does not support -Os; append -O3 to MinSizeRel builds to override it
                set(linker_append ";$<$<CONFIG:MinSizeRel>:-O3>")
            elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                set(cxx_append ";-fno-fat-lto-objects")
            endif()

            if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND prefer_thin_lto)
                _pybind11_return_if_cxx_and_linker_flags_work(HAS_FLTO_THIN
                    "-flto=thin${cxx_append}" "-flto=thin${linker_append}"
                    PYBIND11_LTO_CXX_FLAGS PYBIND11_LTO_LINKER_FLAGS)
            endif()

            if (NOT HAS_FLTO_THIN)
                _pybind11_return_if_cxx_and_linker_flags_work(HAS_FLTO
                    "-flto${cxx_append}" "-flto${linker_append}"
                    PYBIND11_LTO_CXX_FLAGS PYBIND11_LTO_LINKER_FLAGS)
            endif()
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
            # Intel equivalent to LTO is called IPO
            _pybind11_return_if_cxx_and_linker_flags_work(HAS_INTEL_IPO
                "-ipo" "-ipo" PYBIND11_LTO_CXX_FLAGS PYBIND11_LTO_LINKER_FLAGS)
        elseif(MSVC)
            # cmake only interprets libraries as linker flags when they start with a - (otherwise it
            # converts /LTCG to \LTCG as if it was a Windows path).  Luckily MSVC supports passing flags
            # with - instead of /, even if it is a bit non-standard:
            _pybind11_return_if_cxx_and_linker_flags_work(HAS_MSVC_GL_LTCG
                "/GL" "-LTCG" PYBIND11_LTO_CXX_FLAGS PYBIND11_LTO_LINKER_FLAGS)
        endif()

        if (PYBIND11_LTO_CXX_FLAGS)
            message(STATUS "LTO enabled")
        else()
            message(STATUS "LTO disabled (not supported by the compiler and/or linker)")
        endif()
    endif()

    # Enable LTO flags if found, except for Debug builds
    if (PYBIND11_LTO_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE "$<$<NOT:$<CONFIG:Debug>>:${PYBIND11_LTO_CXX_FLAGS}>")
    endif()
    if(PYBIND11_LTO_LINKER_FLAGS)
        target_link_libraries(${target_name} PRIVATE "$<$<NOT:$<CONFIG:Debug>>:${PYBIND11_LTO_LINKER_FLAGS}>")
    endif()
endfunction()

find_package(Sanitizers)

add_executable(kgedata_test ${TEST_SOURCES} ${PY_SOURCES} ${SOURCES})
target_compile_features(kgedata_test PUBLIC cxx_std_17)
target_link_libraries(kgedata_test PUBLIC Catch2::Catch PRIVATE kgedata_structs pybind11::pybind11 pybind11::embed pybind11::module fmt::fmt)
set_property(TARGET kgedata_test PROPERTY POSITION_INDEPENDENT_CODE ON)
_pybind11_add_lto_flags(kgedata_test 1)
add_sanitizers(kgedata_test)

enable_testing()
add_test(COMMAND bin/kgedata_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)
