#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "kgekit.h"
#include "indexer.h"
#include "translation.h"

namespace pybind11 {
namespace detail {

template <> struct type_caster<kgekit::TripleIndex> {
public:
    /**
     * This macro establishes the name 'inty' in
     * function signatures and declares a local variable
     * 'value' of type inty
     */
    PYBIND11_TYPE_CASTER(kgekit::TripleIndex, _("TripleIndex"));

    /**
     * Conversion part 1 (Python->C++): convert a PyObject into a inty
     * instance or return false upon failure. The second argument
     * indicates whether implicit conversions should be applied.
     */
    bool load(handle src, bool)
    {
        /* Extract PyObject from handle */
        PyObject* source = src.ptr();
        if (!source) {
            return false;
        }

        PyObject* head = PyObject_GetAttrString(source, "head");
        if (!head) {
            Py_DECREF(head);
            return false;
        }
        value.head = PyLong_AsUnsignedLong(head);
        Py_DECREF(head);


        PyObject* relation = PyObject_GetAttrString(source, "relation");
        if (!head) {
            Py_DECREF(relation);
            return false;
        }
        value.relation = PyLong_AsUnsignedLong(relation);
        Py_DECREF(relation);

        PyObject* tail = PyObject_GetAttrString(source, "tail");
        if (!tail) {
            Py_DECREF(tail);
            return false;
        }
        value.tail = PyLong_AsUnsignedLong(tail);
        Py_DECREF(tail);

        return PyErr_Occurred();
    }

    /**
     * Conversion part 2 (C++ -> Python): convert an inty instance into
     * a Python object. The second and third arguments are used to
     * indicate the return value policy and parent object (for
     * ``return_value_policy::reference_internal``) and are generally
     * ignored by implicit casters.
     */
    static handle cast(kgekit::TripleIndex src, return_value_policy /* policy */, handle /* parent */)
    {
        return Py_BuildValue("(kkk)", src.head, src.relation, src.tail);
    }
};


template <> struct type_caster<kgekit::Triple> {
public:
    /**
     * This macro establishes the name 'inty' in
     * function signatures and declares a local variable
     * 'value' of type inty
     */
    PYBIND11_TYPE_CASTER(kgekit::Triple, _("Triple"));

    /**
     * Conversion part 1 (Python->C++): convert a PyObject into a inty
     * instance or return false upon failure. The second argument
     * indicates whether implicit conversions should be applied.
     */
    bool load(handle src, bool)
    {
        /* Extract PyObject from handle */
        PyObject* source = src.ptr();
        if (!source) {
            return false;
        }

        PyObject* head = PyList_GetItem(source, 0);
        if (!head) {
            Py_DECREF(head);
            return false;
        }
        auto head_str = PyUnicode_AsUTF8AndSize(head, nullptr);
        value.head = std::string(head_str);
        Py_DECREF(head);

        PyObject* relation = PyObject_GetAttrString(source, "relation");
        if (!head) {
            Py_DECREF(relation);
            return false;
        }
        auto rel_str = PyUnicode_AsUTF8AndSize(relation, nullptr);
        value.relation = std::string(rel_str);
        Py_DECREF(relation);

        PyObject* tail = PyObject_GetAttrString(source, "tail");
        if (!tail) {
            Py_DECREF(tail);
            return false;
        }
        auto tail_str = PyUnicode_AsUTF8AndSize(tail, nullptr);
        value.tail = std::string(tail_str);
        Py_DECREF(tail);

        return PyErr_Occurred();
    }

    /**
     * Conversion part 2 (C++ -> Python): convert an inty instance into
     * a Python object. The second and third arguments are used to
     * indicate the return value policy and parent object (for
     * ``return_value_policy::reference_internal``) and are generally
     * ignored by implicit casters.
     */
    static handle cast(kgekit::Triple src, return_value_policy /* policy */, handle /* parent */)
    {
        return Py_BuildValue("(sss)", src.head.c_str(), src.relation.c_str(), src.tail.c_str());
    }
};

} // namespace detail
} // namespace pybind11


// PYBIND11_MAKE_OPAQUE
namespace pybind11 {
    namespace detail {
        template<> class type_caster<std::unordered_map<std::string, uint32_t>> : public type_caster_base<std::unordered_map<std::string, uint32_t>> {};
        template<> class type_caster<std::vector<kgekit::TripleIndex>> : public type_caster_base<std::vector<kgekit::TripleIndex>> {};
        template<> class type_caster<std::vector<kgekit::Triple>> : public type_caster_base<std::vector<kgekit::Triple>> {};
        template<> class type_caster<std::vector<std::string>> : public type_caster_base<std::vector<std::string>> {};
    }
}

namespace py = pybind11;
/*
 * The naming convention of this file is to help mix with Python code.
 */
PYBIND11_MODULE(_kgekit, m) {
    py::bind_map<std::unordered_map<std::string, uint32_t>>(m, "EntryIdMap");
    py::bind_vector<std::vector<kgekit::TripleIndex>>(m, "TripleIndexList");
    py::bind_vector<std::vector<kgekit::Triple>>(m, "TripleList");

    py::bind_vector<std::vector<std::string>>(m, "EntryList");
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
        .def(py::init<const std::vector<kgekit::Triple>&, const std::string&>())
        .def("getEntityIdMap", &kgekit::EntityNumberIndexer::getEntityIdMap)
        .def("getRelationIdMap", &kgekit::EntityNumberIndexer::getRelationIdMap)
        .def("getIndex", &kgekit::EntityNumberIndexer::getIndex)
        .def("getEntities", &kgekit::EntityNumberIndexer::getEntities)
        .def("getRelations", &kgekit::EntityNumberIndexer::getRelations);


    auto translation = m.def_submodule("translation", "translation service for kgekit");
    translation.def("get_entity_from_id", &kgekit::get_entity_from_id, "gets the entity name from id",
                    py::arg("indexer"),
                    py::arg("id"));
    translation.def("get_relation_from_id", &kgekit::get_relation_from_id, "gets the relation name from id",
                    py::arg("indexer"),
                    py::arg("id"));
    translation.def("get_id_from_entity", &kgekit::get_entity_from_id, "gets the entity id from name",
                    py::arg("indexer"),
                    py::arg("entity"));
    translation.def("get_id_from_relation", &kgekit::get_entity_from_id, "gets the entity name from id",
                    py::arg("indexer"),
                    py::arg("relation"));
}
