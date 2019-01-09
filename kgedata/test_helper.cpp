#include "test_helper.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace py = pybind11;

/*
 * Used for testing. Data structure has to be copied from main module since
 * embedded interpreter can't access them.
 */

PYBIND11_EMBEDDED_MODULE(kgedata_testing, m) {
  py::class_<kgedata::Triple>(m, "Triple")
      .def(py::init<>())
      .def(py::init<const std::string &, const std::string &, const std::string &>())
      .def("__repr__", &kgedata::Triple::repr)
      .def("__eq__", &kgedata::Triple::operator==)
      .def_readwrite("head", &kgedata::Triple::head)
      .def_readwrite("relation", &kgedata::Triple::relation)
      .def_readwrite("tail", &kgedata::Triple::tail);
  py::class_<kgedata::TripleIndex>(m, "TripleIndex")
      .def(py::init<>())
      .def(py::init<int32_t, int32_t, int32_t>())
      .def("__repr__", &kgedata::TripleIndex::repr)
      .def("__eq__", &kgedata::TripleIndex::operator==)
      .def_readwrite("head", &kgedata::TripleIndex::head)
      .def_readwrite("relation", &kgedata::TripleIndex::relation)
      .def_readwrite("tail", &kgedata::TripleIndex::tail);
}
