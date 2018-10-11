#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "kgekit.h"
#include "entity_number_indexer.h"
#include "indexer.h"
#include "translation.h"

namespace py = pybind11;
/*
 * The naming convention of this file is to help mix with Python code.
 */
PYBIND11_MODULE(_kgekit, m) {
    /*
     * C++ data structure are properly binded to Python.
     * For the list, it's easier to unwrapped in C++ so we will keep
     * their wrapper from Python (pybind11).
     */
    py::class_<kgekit::Triple>(m, "Triple")
        .def(py::init<>())
        .def(py::init<std::array<std::string, 3>>())
        .def("__repr__", &kgekit::Triple::repr)
        .def_readwrite("head", &kgekit::Triple::head)
        .def_readwrite("relation", &kgekit::Triple::tail)
        .def_readwrite("tail", &kgekit::Triple::tail);
    py::class_<kgekit::TripleIndex>(m, "TripleIndex")
        .def(py::init<>())
        .def(py::init<std::array<uint32_t, 3>>())
        .def("__repr__", &kgekit::TripleIndex::repr)
        .def_readwrite("head", &kgekit::TripleIndex::head)
        .def_readwrite("relation", &kgekit::TripleIndex::tail)
        .def_readwrite("tail", &kgekit::TripleIndex::tail);

    m.def("get_triple_index", &kgekit::get_triple_index, "get triple index from a line",
          py::arg("line"),
          py::arg("order"),
          py::arg("delimiter"),
          py::arg("skip_checking_order") = false);
    m.def("get_triple", &kgekit::get_triple, "get triple from a line",
          py::arg("line"),
          py::arg("order"),
          py::arg("delimiter"),
          py::arg("skip_checking_order") = false);

    py::class_<kgekit::EntityNumberIndexer, std::shared_ptr<kgekit::EntityNumberIndexer>>(m, "EntityNumberIndexer")
        .def(py::init<const py::list&, const std::string&>())
        .def("entityIdMap", &kgekit::EntityNumberIndexer::entityIdMap)
        .def("relationIdMap", &kgekit::EntityNumberIndexer::relationIdMap)
        .def("indexes", &kgekit::EntityNumberIndexer::indexes)
        .def("entities", &kgekit::EntityNumberIndexer::entities)
        .def("relations", &kgekit::EntityNumberIndexer::relations);


    // auto translation = m.def_submodule("translation", "translation service for kgekit");
    // translation.def("get_entity_from_id", &kgekit::get_entity_from_id, "gets the entity name from id",
    //                 py::arg("indexer"),
    //                 py::arg("id"));
    // translation.def("get_relation_from_id", &kgekit::get_relation_from_id, "gets the relation name from id",
    //                 py::arg("indexer"),
    //                 py::arg("id"));
    // translation.def("get_id_from_entity", &kgekit::get_entity_from_id, "gets the entity id from name",
    //                 py::arg("indexer"),
    //                 py::arg("entity"));
    // translation.def("get_id_from_relation", &kgekit::get_entity_from_id, "gets the entity name from id",
    //                 py::arg("indexer"),
    //                 py::arg("relation"));
}
