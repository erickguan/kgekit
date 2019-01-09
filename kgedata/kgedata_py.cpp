#include <random>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "kgedata.h"
#include "entity_number_indexer.h"
#include "lcwa_no_throw_sampler.h"
#include "bernoulli_corruptor.h"
#include "ranker.h"
#include "triple_expander.h"

namespace py = pybind11;

/*
 * The naming convention of this file is to help mix with Python code.
 */
PYBIND11_MODULE(kgedata, m) {
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

    m.def("get_triple_index", &kgedata::get_triple_index, "get triple index from a line",
          py::arg("line"),
          py::arg("order"),
          py::arg("delimiter"),
          py::arg("skip_checking_order") = false);
    m.def("get_triple", &kgedata::get_triple, "get triple from a line",
          py::arg("line"),
          py::arg("order"),
          py::arg("delimiter"),
          py::arg("skip_checking_order") = false);

    m.def("expand_triple_batch", &kgedata::expand_triple_batch, "expands triple batch",
          py::arg("batch"),
          py::arg("num_entity"),
          py::arg("num_relation"),
          py::arg("expand_entity"),
          py::arg("expand_relation"));

    py::class_<kgedata::EntityNumberIndexer>(m, "EntityNumberIndexer", "index from triples and translation between index and lists")
        .def(py::init<const py::list&, const std::string&>())
        .def("entityIdMap", &kgedata::EntityNumberIndexer::entityIdMap)
        .def("relationIdMap", &kgedata::EntityNumberIndexer::relationIdMap)
        .def("indexes", &kgedata::EntityNumberIndexer::indexes, "gets indexes from triples")
        .def("entities", &kgedata::EntityNumberIndexer::entities, "gets entity list")
        .def("relations", &kgedata::EntityNumberIndexer::relations, "gets relation list")
        .def("getEntityFromId", &kgedata::EntityNumberIndexer::getEntityFromId, "gets the entity name from id")
        .def("getRelationFromId", &kgedata::EntityNumberIndexer::getRelationFromId, "gets the relation name from id")
        .def("getIdFromEntity", &kgedata::EntityNumberIndexer::getIdFromEntity, "gets the entity id from name")
        .def("getIdFromRelation", &kgedata::EntityNumberIndexer::getIdFromRelation, "gets the entity name from id");

    py::enum_<kgedata::LCWANoThrowSampler::Strategy>(m, "LCWANoThrowSamplerStrategy", py::arithmetic())
        .value("Hash", kgedata::LCWANoThrowSampler::Strategy::Hash)
        .value("Offset", kgedata::LCWANoThrowSampler::Strategy::Offset);

    py::class_<kgedata::LCWANoThrowSampler>(m, "LCWANoThrowSampler")
        .def(py::init<const py::list&, int64_t, int64_t, int16_t, int16_t, kgedata::LCWANoThrowSampler::Strategy>())
        .def("numNegativeSamples", &kgedata::LCWANoThrowSampler::numNegativeSamples, "gets the number of negative samples")
        .def("sample", &kgedata::LCWANoThrowSampler::sample, py::arg("arr").noconvert(), py::arg("corrupt_head_list").noconvert(), py::arg("batch").noconvert(), py::arg("random_seed") = std::random_device{}(), "samples current batch");

    py::class_<kgedata::BernoulliCorruptor>(m, "BernoulliCorruptor", "generates the bernoulli distribution of samples")
        .def(py::init<const py::list&>())
        .def("getProbablityRelation", &kgedata::BernoulliCorruptor::getProbablityRelation, "gets the probablity pair of given relation");

    py::class_<kgedata::Ranker>(m, "Ranker", "ranks the prediction")
        .def(py::init<const py::list&, const py::list&, const py::list&>())
        .def("rankHead", &kgedata::Ranker::rankHead, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test head prediction.")
        .def("rankTail", &kgedata::Ranker::rankTail, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test tail prediction.")
        .def("rankRelation", &kgedata::Ranker::rankRelation, py::arg("arr").noconvert(), py::arg("triple").noconvert(), "returns rank and filter rank of triple of interests. Used to test relation prediction.")
        .def("exportState", &kgedata::Ranker::exportState)
        .def(py::pickle(&kgedata::ranker_pickle_getstate, &kgedata::ranker_pickle_setstate));
}

