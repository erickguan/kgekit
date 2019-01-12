#pragma once

#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <memory>
#include <functional>
#include <cstdint>

#include <boost/core/noncopyable.hpp>
#include <pybind11/numpy.h>

#include "kgedata.h"

namespace kgedata {

using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::make_pair;
using std::unique_ptr;
using std::make_unique;

namespace py = pybind11;

/*
 * PyTorch requires LongTensor for indicies so int64_t is used.
 */
class LCWANoThrowSampler: private boost::noncopyable {
public:
    enum class Strategy {
        Hash,
        Offset
    };
    LCWANoThrowSampler(
        const py::array_t<int64_t>& train_set,
        int64_t num_entity,
        int64_t num_relation,
        int16_t num_corrupt_entity,
        int16_t num_corrupt_relation,
        int64_t random_seed=std::random_device{}(),
        Strategy strategy=Strategy::Hash);
    int16_t numNegativeSamples() const;
    /* In place editing. Avoid copies for large elements */
    py::array_t<int64_t, py::array::c_style> sample(
        py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
        py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
        bool nested_batch=false);
private:
    Strategy Strategy_;
    std::mt19937_64 random_engine_;
    int16_t num_corrupt_entity_;
    int16_t num_corrupt_relation_;
    struct SampleStrategy {
        virtual ~SampleStrategy() {};
        virtual py::array_t<int64_t, py::array::c_style> sample(
            py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
            py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
            bool nested_batch) = 0;
    };
    class HashSampleStrategy : public SampleStrategy {
    public:
        HashSampleStrategy(const py::array_t<int64_t>& triples, LCWANoThrowSampler* sampler);
        py::array_t<int64_t, py::array::c_style> sample(
            py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
            py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
            bool nested_batch) override;
    private:
        int64_t generateCorruptHead(int64_t h, int64_t r, const std::function<int64_t(void)>& generate_random_func);
        int64_t generateCorruptTail(int64_t t, int64_t r, const std::function<int64_t(void)>& generate_random_func);
        int64_t generateCorruptRelation(int64_t h, int64_t t, const std::function<int64_t(void)>& generate_random_func);
        LCWANoThrowSampler* sampler_;

        unordered_map<int64_t, unordered_set<int64_t>> rest_head_;
        unordered_map<int64_t, unordered_set<int64_t>> rest_tail_;
        unordered_map<int64_t, unordered_set<int64_t>> rest_relation_;
    };
    unique_ptr<SampleStrategy> sample_strategy_;
    int64_t num_entity_ = -1;
    int64_t num_relation_ = -1;
};

} // namespace kgedata
