#include <random>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "kgekit.h"
#include "entity_number_indexer.h"
#include "lcwa_no_throw_sampler.h"
#include "bernoulli_corruptor.h"
#include "ranker.h"
#include "triple_expander.h"

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
        .def(py::init<const std::string&, const std::string&, const std::string&>())
        .def("__repr__", &kgekit::Triple::repr)
        .def("__eq__", &kgekit::Triple::operator==)
        .def("serialize", &kgekit::Triple::serialize)
        .def_readwrite("head", &kgekit::Triple::head)
        .def_readwrite("relation", &kgekit::Triple::relation)
        .def_readwrite("tail", &kgekit::Triple::tail)
        .def(py::pickle(
            [](const kgekit::Triple &p) { // __getstate__
                return py::make_tuple(p.head, p.relation, p.tail);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 3) {
                    throw std::runtime_error("Invalid state!");
                }
                return kgekit::Triple(t[0].cast<std::string>(), t[1].cast<std::string>(), t[2].cast<std::string>());
            }
        ));

    py::class_<kgekit::TripleIndex>(m, "TripleIndex")
        .def(py::init<>())
        .def(py::init<int32_t, int32_t, int32_t>())
        .def("__repr__", &kgekit::TripleIndex::repr)
        .def("__eq__", &kgekit::TripleIndex::operator==)
        .def("serialize", &kgekit::TripleIndex::serialize)
        .def_readwrite("head", &kgekit::TripleIndex::head)
        .def_readwrite("relation", &kgekit::TripleIndex::relation)
        .def_readwrite("tail", &kgekit::TripleIndex::tail)
        .def(py::pickle(
            [](const kgekit::TripleIndex &p) { // __getstate__
                return py::make_tuple(p.head, p.relation, p.tail);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 3) {
                    throw std::runtime_error("Invalid state!");
                }
                return kgekit::TripleIndex(t[0].cast<int32_t>(), t[1].cast<int32_t>(), t[2].cast<int32_t>());
            }
        ));

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

    m.def("expand_triple_batch", &kgekit::expand_triple_batch, "expands triple batch",
          py::arg("batch"),
          py::arg("num_entity"),
          py::arg("num_relation"),
          py::arg("expand_entity"),
          py::arg("expand_relation"));

    py::class_<kgekit::EntityNumberIndexer>(m, "EntityNumberIndexer", "index from triples and translation between index and lists")
        .def(py::init<const py::list&, const std::string&>())
        .def("entityIdMap", &kgekit::EntityNumberIndexer::entityIdMap)
        .def("relationIdMap", &kgekit::EntityNumberIndexer::relationIdMap)
        .def("indexes", &kgekit::EntityNumberIndexer::indexes, "gets indexes from triples")
        .def("entities", &kgekit::EntityNumberIndexer::entities, "gets entity list")
        .def("relations", &kgekit::EntityNumberIndexer::relations, "gets relation list")
        .def("getEntityFromId", &kgekit::EntityNumberIndexer::getEntityFromId, "gets the entity name from id")
        .def("getRelationFromId", &kgekit::EntityNumberIndexer::getRelationFromId, "gets the relation name from id")
        .def("getIdFromEntity", &kgekit::EntityNumberIndexer::getIdFromEntity, "gets the entity id from name")
        .def("getIdFromRelation", &kgekit::EntityNumberIndexer::getIdFromRelation, "gets the entity name from id");

    py::enum_<kgekit::LCWANoThrowSampler::Strategy>(m, "LCWANoThrowSamplerStrategy", py::arithmetic())
        .value("Hash", kgekit::LCWANoThrowSampler::Strategy::Hash)
        .value("Offset", kgekit::LCWANoThrowSampler::Strategy::Offset);

    py::class_<kgekit::LCWANoThrowSampler>(m, "LCWANoThrowSampler")
        .def(py::init<const py::list&, int64_t, int64_t, int16_t, int16_t, kgekit::LCWANoThrowSampler::Strategy>())
        .def("numNegativeSamples", &kgekit::LCWANoThrowSampler::numNegativeSamples, "gets the number of negative samples")
        .def("sample", &kgekit::LCWANoThrowSampler::sample, py::arg("arr").noconvert(), py::arg("corrupt_head_list").noconvert(), py::arg("batch").noconvert(), py::arg("random_seed") = std::random_device{}(), "samples current batch");

    py::class_<kgekit::BernoulliCorruptor>(m, "BernoulliCorruptor", "generates the bernoulli distribution of samples")
        .def(py::init<const py::list&>())
        .def("getProbablityRelation", &kgekit::BernoulliCorruptor::getProbablityRelation, "gets the probablity pair of given relation");

    py::class_<kgekit::Ranker>(m, "Ranker", "ranks the prediction")
        .def(py::init<const py::list&, const py::list&, const py::list&>())
        .def("rankHead", &kgekit::Ranker::rankHead, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test head prediction.")
        .def("rankTail", &kgekit::Ranker::rankTail, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test tail prediction.")
        .def("rankRelation", &kgekit::Ranker::rankRelation, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test relation prediction.")
        .def("exportState", &kgekit::Ranker::exportState)
        .def(py::pickle(&kgekit::ranker_pickle_getstate, &kgekit::ranker_pickle_setstate));
}

