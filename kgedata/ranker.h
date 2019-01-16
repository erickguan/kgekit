#pragma once

#include <utility>
#include <unordered_set>
#include <unordered_map>

#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "kgedata.h"

namespace kgedata {

using std::unordered_set;
using std::unordered_map;
using std::make_pair;
using std::pair;

namespace py = pybind11;

class Ranker {
public:
    Ranker(const py::array_t<int64_t>& train_triples, const py::array_t<int64_t>& valid_triples, const py::array_t<int64_t>& test_triples);
    Ranker(const py::tuple& rest_head, const py::tuple& rest_tail, const py::tuple& rest_relation);
    pair<int32_t, int32_t> rankHead(py::array_t<float>& arr, const TripleIndex& triple);
    pair<int32_t, int32_t> rankTail(py::array_t<float>& arr, const TripleIndex& triple);
    pair<int32_t, int32_t> rankRelation(py::array_t<float>& arr, const TripleIndex& triple);
    py::tuple exportState() const;
private:
    unordered_map<int64_t, unordered_set<int64_t>> rest_head_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_tail_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_relation_;
};

py::tuple ranker_pickle_getstate(const Ranker& ranker);
Ranker ranker_pickle_setstate(py::tuple t);

} // namespace kgedata
