#pragma once

#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <new>

#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "kgedata.h"

namespace kgedata {

using std::unordered_set;
using std::unordered_map;
using std::make_pair;
using std::pair;
using std::make_shared;
using std::shared_ptr;

namespace py = pybind11;

optional<pair<int32_t, int32_t>> rank_raw(
  const float* begin,
  const float* end,
  int64_t original,
  unordered_set<int64_t>& filters,
  bool ascending_rank=true);

class Ranker {
public:
    Ranker(
        const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& train_triples,
        const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& valid_triples,
        const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& test_triples);
    Ranker(const py::tuple& rest_head, const py::tuple& rest_tail, const py::tuple& rest_relation);
    pair<int32_t, int32_t> rank_head(
        py::array_t<float>& arr,
        py::array_t<int64_t>& triple,
        bool ascending_rank = true);
    pair<int32_t, int32_t> rank_tail(
        py::array_t<float>& arr,
        py::array_t<int64_t>& triple,
        bool ascending_rank = true);
    pair<int32_t, int32_t> rank_relation(
        py::array_t<float>& arr,
        py::array_t<int64_t>& triple,
        bool ascending_rank = true);
    optional<pair<int32_t, int32_t>> rank_head_raw(
        const float* begin,
        const float* end,
        const TripleIndex& triple,
        bool ascending_rank = true);
    optional<pair<int32_t, int32_t>> rank_tail_raw(
        const float* begin,
        const float* end,
        const TripleIndex& triple,
        bool ascending_rank = true);
    optional<pair<int32_t, int32_t>> rank_relation_raw(
        const float* begin,
        const float* end,
        const TripleIndex& triple,
        bool ascending_rank = true);
    py::tuple export_state() const;
    py::array_t<int64_t, py::array::c_style> submit(
        py::array_t<float, py::array::c_style | py::array::forcecast> prediction,
        py::array_t<int64_t, py::array::c_style | py::array::forcecast> triple,
        py::array_t<int64_t, py::array::c_style | py::array::forcecast> split_points,
        bool ascending_rank=true);
private:
    unordered_map<int64_t, unordered_set<int64_t>> rest_head_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_tail_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_relation_;
};


py::tuple ranker_pickle_getstate(const Ranker& ranker);
Ranker ranker_pickle_setstate(py::tuple t);

} // namespace kgedata
