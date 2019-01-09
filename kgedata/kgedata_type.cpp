/*
 * C++ data structure are properly binded to Python.
 * For the list, it's easier to unwrapped in C++ so we will keep
 * their wrapper from Python (pybind11).
 */

py::class_<kgedata::Triple>(m, "Triple")
    .def(py::init<>())
    .def(py::init<const std::string&, const std::string&, const std::string&>())
    .def("__repr__", &kgedata::Triple::repr)
    .def("__eq__", &kgedata::Triple::operator==)
    .def("serialize", &kgedata::Triple::serialize)
    .def_readwrite("head", &kgedata::Triple::head)
    .def_readwrite("relation", &kgedata::Triple::relation)
    .def_readwrite("tail", &kgedata::Triple::tail)
    .def(py::pickle(
        [](const kgedata::Triple &p) { // __getstate__
            return py::make_tuple(p.head, p.relation, p.tail);
        },
        [](py::tuple t) { // __setstate__
            if (t.size() != 3) {
                throw std::runtime_error("Invalid state!");
            }
            return kgedata::Triple(t[0].cast<std::string>(), t[1].cast<std::string>(), t[2].cast<std::string>());
        }
    ));

py::class_<kgedata::TripleIndex>(m, "TripleIndex")
    .def(py::init<>())
    .def(py::init<int32_t, int32_t, int32_t>())
    .def("__repr__", &kgedata::TripleIndex::repr)
    .def("__eq__", &kgedata::TripleIndex::operator==)
    .def("serialize", &kgedata::TripleIndex::serialize)
    .def_readwrite("head", &kgedata::TripleIndex::head)
    .def_readwrite("relation", &kgedata::TripleIndex::relation)
    .def_readwrite("tail", &kgedata::TripleIndex::tail)
    .def(py::pickle(
        [](const kgedata::TripleIndex &p) { // __getstate__
            return py::make_tuple(p.head, p.relation, p.tail);
        },
        [](py::tuple t) { // __setstate__
            if (t.size() != 3) {
                throw std::runtime_error("Invalid state!");
            }
            return kgedata::TripleIndex(t[0].cast<int32_t>(), t[1].cast<int32_t>(), t[2].cast<int32_t>());
        }
    ));
