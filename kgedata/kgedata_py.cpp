#include <random>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "corruptors.h"
#include "entity_number_indexer.h"
#include "kgedata.h"
#include "label_generator.h"
#include "negative_samplers.h"
#include "ranker.h"
#include "triple_expander.h"

namespace py = pybind11;

/*
 * The naming convention of this file is to help mix with Python code.
 */
PYBIND11_MODULE(kgedata, m)
{
#include "kgedata_type.cpp"

  m.def("get_triple_index", &kgedata::get_triple_index,
        "get triple index from a line", py::arg("line"), py::arg("order"),
        py::arg("delimiter"), py::arg("skip_checking_order") = false);
  m.def("get_triple", &kgedata::get_triple, "get triple from a line",
        py::arg("line"), py::arg("order"), py::arg("delimiter"),
        py::arg("skip_checking_order") = false);

  m.def("expand_triple_batch", &kgedata::expand_triple_batch,
        "expands triple batch", py::arg("batch"), py::arg("num_entity"),
        py::arg("num_relation"), py::arg("expand_entity") = true,
        py::arg("expand_relation") = false);

  py::class_<kgedata::EntityNumberIndexer>(
      m, "EntityNumberIndexer",
      "index from triples and translation between index and lists")
      .def(py::init<const py::list&, const std::string&>())
      .def("entityIdMap", &kgedata::EntityNumberIndexer::entityIdMap)
      .def("relationIdMap", &kgedata::EntityNumberIndexer::relationIdMap)
      .def("indexes", &kgedata::EntityNumberIndexer::indexes,
           "gets indexes from triples")
      .def("entities", &kgedata::EntityNumberIndexer::entities,
           "gets entity list")
      .def("relations", &kgedata::EntityNumberIndexer::relations,
           "gets relation list")
      .def("getEntityFromId", &kgedata::EntityNumberIndexer::getEntityFromId,
           "gets the entity name from id")
      .def("getRelationFromId",
           &kgedata::EntityNumberIndexer::getRelationFromId,
           "gets the relation name from id")
      .def("getIdFromEntity", &kgedata::EntityNumberIndexer::getIdFromEntity,
           "gets the entity id from name")
      .def("getIdFromRelation",
           &kgedata::EntityNumberIndexer::getIdFromRelation,
           "gets the entity name from id");

  py::enum_<kgedata::PerturbationSampler::Strategy>(
      m, "PerturbationSamplerStrategy", py::arithmetic())
      .value("Hash", kgedata::PerturbationSampler::Strategy::Hash)
      .value("Offset", kgedata::PerturbationSampler::Strategy::Offset);

  py::class_<kgedata::PerturbationSampler>(m, "PerturbationSampler")
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    int64_t, int64_t, int16_t, int16_t>())
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    int64_t, int64_t, int16_t, int16_t, int64_t>())
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    int64_t, int64_t, int16_t, int16_t, int64_t,
                    kgedata::PerturbationSampler::Strategy>())
      .def("numNegativeSamples",
           &kgedata::PerturbationSampler::numNegativeSamples,
           "gets the number of negative samples")
      .def("__call__", &kgedata::PerturbationSampler::sample,
           py::arg("corrupt_head_flags").noconvert(),
           py::arg("batch").noconvert(), "samples current batch")
      .def("sample", &kgedata::PerturbationSampler::sample,
           py::arg("corrupt_head_flags").noconvert(),
           py::arg("batch").noconvert(), "samples current batch");

  py::class_<kgedata::CWASampler>(m, "CWASampler",
                                  "CWASampler returns a negative batch. "
                                  "Supports flags for corrupt relation too.")
      .def(py::init<int64_t, int64_t, bool>())
      .def("__call__", &kgedata::CWASampler::sample,
           py::arg("corrupt_head_flags").noconvert(),
           py::arg("batch").noconvert(), "samples current batch")
      .def("sample", &kgedata::CWASampler::sample,
           py::arg("corrupt_head_flags").noconvert(),
           py::arg("batch").noconvert(), "samples current batch");

  py::class_<kgedata::MemoryLabelGenerator>(
      m, "MemoryLabelGenerator",
      "MemoryLabelGenerator returns labels based on if the given triple "
      "appears on "
      "the training set")
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    float, float>())
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&>())
      .def("__call__", &kgedata::MemoryLabelGenerator::generate_labels,
           py::arg("batch").noconvert(), "generates labels for the batch")
      .def("generate_labels", &kgedata::MemoryLabelGenerator::generate_labels,
           py::arg("batch").noconvert(), "generates labels for the batch");
  py::class_<kgedata::StaticLabelGenerator>(
      m, "StaticLabelGenerator",
      "StaticLabelGenerator returns labels based on set parameters.")
      .def(py::init<bool, float, float>())

      .def(py::init<bool>())
      .def(
          "__call__",
          py::overload_cast<
              py::array_t<int64_t, py::array::c_style | py::array::forcecast>&>(
              &kgedata::StaticLabelGenerator::generate_labels),
          py::arg("batch").noconvert(), "generates labels for the batch")
      .def(
          "generate_labels",
          py::overload_cast<
              py::array_t<int64_t, py::array::c_style | py::array::forcecast>&>(
              &kgedata::StaticLabelGenerator::generate_labels),
          py::arg("batch").noconvert(), "generates labels for the batch")
      .def("__call__",
           py::overload_cast<py::tuple>(
               &kgedata::StaticLabelGenerator::generate_labels),
           py::arg("shape").noconvert(), "generates labels for the batch")
      .def("generate_labels",
           py::overload_cast<py::tuple>(
               &kgedata::StaticLabelGenerator::generate_labels),
           py::arg("shape").noconvert(), "generates labels for the batch");

  py::class_<kgedata::BernoulliCorruptor>(
      m, "BernoulliCorruptor",
      "generates the bernoulli distribution of samples")
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    int32_t, int64_t>())
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    int32_t, int64_t, int64_t>())
      .def("get_probability_relation",
           &kgedata::BernoulliCorruptor::get_probability_relation,
           "gets the probability for certain relation")
      .def("__call__", &kgedata::BernoulliCorruptor::make_random_choice,
           py::arg("batch").noconvert(), "gets the choice for given batch item")
      .def("make_random_choice",
           &kgedata::BernoulliCorruptor::make_random_choice,
           py::arg("batch").noconvert(),
           "gets the choice for given batch item");
  py::class_<kgedata::UniformCorruptor>(
      m, "UniformCorruptor", "generates the uniform distribution of samples")
      .def(py::init<int64_t>())
      .def(py::init<int64_t, int64_t>())
      .def("__call__", &kgedata::UniformCorruptor::make_random_choice,
           py::arg("batch").noconvert(), "gets the choice for given batch item")
      .def("make_random_choice", &kgedata::UniformCorruptor::make_random_choice,
           py::arg("batch").noconvert(),
           "gets the choice for given batch item");
  py::class_<kgedata::StaticCorruptor>(
      m, "StaticCorruptor", "generates the same corruption results of samples")
      .def(py::init<int64_t>())
      .def(py::init<int64_t, bool>())
      .def("__call__", &kgedata::StaticCorruptor::make_choice,
           py::arg("batch").noconvert(), "gets the choice for given batch item")
      .def("make_choice", &kgedata::StaticCorruptor::make_choice,
           py::arg("batch").noconvert(),
           "gets the choice for given batch item");

  py::class_<kgedata::Ranker>(m, "Ranker", "ranks the prediction")
      .def(py::init<const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&,
                    const py::array_t<int64_t, py::array::c_style |
                                                   py::array::forcecast>&>())
      .def("rank_head", &kgedata::Ranker::rank_head, py::arg("arr").noconvert(),
           py::arg("triple").noconvert(),
           py::arg("ascending_rank").noconvert() = true,
           "returns rank and filter rank of triple of interests. Used to test "
           "head prediction.")
      .def("rank_tail", &kgedata::Ranker::rank_tail, py::arg("arr").noconvert(),
           py::arg("triple").noconvert(),
           py::arg("ascending_rank").noconvert() = true,
           "returns rank and filter rank of triple of interests. Used to test "
           "tail prediction.")
      .def("rank_relation", &kgedata::Ranker::rank_relation,
           py::arg("arr").noconvert(), py::arg("triple").noconvert(),
           py::arg("ascending_rank").noconvert() = true,
           "returns rank and filter rank of triple of interests. Used to test "
           "relation prediction.")
      .def("export_state", &kgedata::Ranker::export_state)
      .def("submit", &kgedata::Ranker::submit,
           py::arg("prediction").noconvert(), py::arg("triple").noconvert(),
           py::arg("split_points").noconvert(),
           py::arg("ascending_rank").noconvert() = true,
           "submits a whole batch and get results")
      .def(py::pickle(&kgedata::ranker_pickle_getstate,
                      &kgedata::ranker_pickle_setstate));
}
