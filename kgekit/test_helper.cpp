#include "test_helper.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

/*
 * Used for testing. Data structure has to be copied from main module since
 * embedded interpreter can't access them.
 */

PYBIND11_EMBEDDED_MODULE(kgekit_testing, m) {
  py::class_<kgekit::Triple>(m, "Triple")
      .def(py::init<>())
      .def(py::init<const std::string &, const std::string &, const std::string &>())
      .def("__repr__", &kgekit::Triple::repr)
      .def("__eq__", &kgekit::Triple::operator==)
      .def_readwrite("head", &kgekit::Triple::head)
      .def_readwrite("relation", &kgekit::Triple::relation)
      .def_readwrite("tail", &kgekit::Triple::tail);
  py::class_<kgekit::TripleIndex>(m, "TripleIndex")
      .def(py::init<>())
      .def(py::init<int32_t, int32_t, int32_t>())
      .def("__repr__", &kgekit::TripleIndex::repr)
      .def("__eq__", &kgekit::TripleIndex::operator==)
      .def_readwrite("head", &kgekit::TripleIndex::head)
      .def_readwrite("relation", &kgekit::TripleIndex::relation)
      .def_readwrite("tail", &kgekit::TripleIndex::tail);
}
