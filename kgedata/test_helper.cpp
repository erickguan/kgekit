#include "test_helper.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

/*
 * Used for testing. Data structure has to be copied from main module since
 * embedded interpreter can't access them.
 */

PYBIND11_EMBEDDED_MODULE(kgedata_testing, m) {
#include "kgedata_type.cpp"
}
