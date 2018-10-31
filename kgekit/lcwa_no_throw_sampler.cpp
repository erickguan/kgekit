#include "lcwa_no_throw_sampler.h"

#include "kgekit.h"

namespace py = pybind11;

namespace kgekit {

LCWANoThrowSampler::LCWANoThrowSampler(const py::list& train_set, int16_t num_negative_entity, int16_t num_negative_relation, bool bernoulli)
    : num_negative_entity_(num_negative_entity), num_negative_relation_(num_negative_relation), bernoulli_(bernoulli)
{
    for (auto const& t : train_set) {
        auto triple = t.cast<TripleIndex>();
        // rest_head[make_pair(triple.relation, triple.tail)] = triple.head;
        // rest_tail[make_pair(triple.head, triple.relation)] = triple.tail;
        // rest_relation[make_pair(triple.head, triple.tail)] = triple.relation;
    }
}

int16_t LCWANoThrowSampler::numNegativeSamples() const
{
    return num_negative_entity_ + num_negative_relation_;
}

void LCWANoThrowSampler::sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& arr, const py::list& batch)
{
    auto r = arr.mutable_unchecked<3>(); // Will throw if ndim != 3 or flags.writeable is false
    for (ssize_t i = 0; i < r.shape(0); i++) {
        for (ssize_t j = 0; j < r.shape(1); j++) {
            for (ssize_t k = 0; k < r.shape(2); k++) {
                r(i, j, k) += 1;
            }
        }
    }
}

} // namespace kgekit
