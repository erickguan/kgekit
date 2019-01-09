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

class Ranker {
public:
    Ranker(const pybind11::list& train_triples, const pybind11::list& valid_triples, const pybind11::list& test_triples);
    Ranker(const pybind11::tuple& rest_head, const pybind11::tuple& rest_tail, const pybind11::tuple& rest_relation);
    pair<int32_t, int32_t> rankHead(pybind11::array_t<float>& arr, const TripleIndex& triple);
    pair<int32_t, int32_t> rankTail(pybind11::array_t<float>& arr, const TripleIndex& triple);
    pair<int32_t, int32_t> rankRelation(pybind11::array_t<float>& arr, const TripleIndex& triple);
    pybind11::tuple exportState() const;
private:
    unordered_map<int64_t, unordered_set<int64_t>> rest_head_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_tail_;
    unordered_map<int64_t, unordered_set<int64_t>> rest_relation_;
};

pybind11::tuple ranker_pickle_getstate(const Ranker& ranker);
Ranker ranker_pickle_setstate(pybind11::tuple t);

} // namespace kgedata
