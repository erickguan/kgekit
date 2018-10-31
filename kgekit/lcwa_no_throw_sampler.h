#pragma once

#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <boost/core/noncopyable.hpp>
#include <pybind11/numpy.h>

namespace kgekit {

using std::unordered_map;
using std::unordered_set;
using std::pair;
using std::make_pair;
using std::unique_ptr;
using std::make_unique;

namespace py = pybind11;

/*
 * Pack two int32_t values into a int64_t
 */
inline int64_t _pack_value(int32_t a, int32_t b)
{
    return (static_cast<int64_t>(a) << 32) + b;
}

class LCWANoThrowSampler: private boost::noncopyable {
public:
    enum class Strategy {
        Hash,
        Offset
    };
    LCWANoThrowSampler(const py::list& train_set, int16_t num_negative_entity, int16_t num_negative_relation, Strategy strategy=Strategy::Hash);
    int16_t numNegativeSamples() const;
    /* In place editing. Avoid copies for large elements */
    void sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& array, const py::list& batch);
private:
    Strategy Strategy_;
    int16_t num_negative_entity_;
    int16_t num_negative_relation_;
    struct SampleStrategy {
        virtual ~SampleStrategy() {};
        virtual void sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& array, const py::list& batch) = 0;
    };
    class HashSampleStrategy : public SampleStrategy {
    public:
        HashSampleStrategy(const py::list& triples, LCWANoThrowSampler* sampler);
        void sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& array, const py::list& batch) override;
    private:
        LCWANoThrowSampler* sampler_;

        unordered_map<int64_t, unordered_set<int32_t>> rest_head_;
        unordered_map<int64_t, unordered_set<int32_t>> rest_tail_;
        unordered_map<int64_t, unordered_set<int32_t>> rest_relation_;
    };
    unique_ptr<SampleStrategy> sample_strategy_;
};

} // namespace kgekit
