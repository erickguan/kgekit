#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "kgekit.h"

namespace py = pybind11;

PYBIND11_MODULE(kgekit, m) {
    m.doc() = "kgekit helps with io and sampling"; // optional module docstring
    m.def("read_triple_index", &kgekit::read_triple_index, "read dataset file includes triple index.",
          py::arg("filename"),
          py::arg("triple_order") = "hrt",
          py::arg("delimiter") = '\t');
    m.def("get_triple_index", &kgekit::get_triple_index, "get triple index from a line",
          py::arg("line"),
          py::arg("order"),
          py::arg("delimiter"));
}
