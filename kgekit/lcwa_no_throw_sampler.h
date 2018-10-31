#pragma once

#include <utility>
#include <unordered_map>

#include <pybind11/numpy.h>

namespace kgekit {

using std::unordered_map;
using std::pair;

namespace py = pybind11;

class LCWANoThrowSampler {
public:
    LCWANoThrowSampler(const py::list& train_set, int16_t num_negative_entity, int16_t num_negative_relation, bool bernoulli);
    LCWANoThrowSampler(const LCWANoThrowSampler&) = delete;
    LCWANoThrowSampler& operator=(const LCWANoThrowSampler&) = delete;
    int16_t numNegativeSamples() const;
    /* In place editing. Avoid copies for large elements */
    void sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& array, const py::list& batch);
private:
    // unordered_map<pair<int16_t, int16_t>, int16_t> rest_head;
    // unordered_map<pair<int16_t, int16_t>, int16_t> rest_tail;
    // unordered_map<pair<int16_t, int16_t>, int16_t> rest_relation;
    int16_t num_negative_entity_;
    int16_t num_negative_relation_;
    bool bernoulli_;
};

} // namespace kgekit
