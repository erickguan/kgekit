#include "lcwa_no_throw_sampler.h"

#include "kgekit.h"

namespace py = pybind11;

namespace kgekit {

LCWANoThrowSampler::LCWANoThrowSampler(const py::list& train_set, int16_t num_negative_entity, int16_t num_negative_relation, Strategy strategy)
    : num_negative_entity_(num_negative_entity), num_negative_relation_(num_negative_relation)
{
    if (strategy == Strategy::Hash) {
        sample_strategy_ = make_unique<LCWANoThrowSampler::HashSampleStrategy>(train_set, this);
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

LCWANoThrowSampler::HashSampleStrategy::HashSampleStrategy(const py::list& triples, LCWANoThrowSampler* sampler)
    : sampler_(sampler)
{
    for (auto const& t : triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[internal::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_relation_[internal::_pack_value(triple.head, triple.tail)].insert(triple.relation);
        rest_tail_[internal::_pack_value(triple.head, triple.relation)].insert(triple.tail);
    }
}

void LCWANoThrowSampler::HashSampleStrategy::sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& array, const py::list& batch)
{

}


} // namespace kgekit
