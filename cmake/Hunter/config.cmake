include(hunter_cmake_args)

hunter_cmake_args(
    fmt
    CMAKE_ARGS
        FMT_DOC=OFF
        FMT_TEST=OFF
        FMT_USE_CPP14=OFF # don't force c++14
        CMAKE_POSITION_INDEPENDENT_CODE=ON
)
