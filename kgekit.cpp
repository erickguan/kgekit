#include <pybind11/pybind11.h>
#include "kgekit.h"

namespace py = pybind11;

PYBIND11_MODULE(kgecpp, m) {
    m.doc() = "kgecpp helps with io and sampling"; // optional module docstring
    m.def("read_triple_index", &kgekit::read_triple_index, "read dataset file includes triple index.",
          py::arg("triple_order") = "hrt",
          py::arg("sperator") = '\t');
    m.def("get_triple_index", &kgekit::get_triple_index, "get triple index from a line",
        py::arg("sperator") = '\t');
}
