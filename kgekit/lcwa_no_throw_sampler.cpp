#include "lcwa_no_throw_sampler.h"

#include <algorithm>

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

void LCWANoThrowSampler::sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& arr, const py::list& batch, int64_t random_seed)
{
    sample_strategy_->sample(arr, batch, random_seed);
}

LCWANoThrowSampler::HashSampleStrategy::HashSampleStrategy(const py::list& triples, LCWANoThrowSampler* sampler)
    : sampler_(sampler)
{
    for (auto const& t : triples) {
        auto triple = t.cast<TripleIndex>();
        max_entity_ = std::max(max_entity_, std::max(triple.head, triple.tail));
        rest_head_[internal::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_relation_[internal::_pack_value(triple.head, triple.tail)].insert(triple.relation);
        rest_tail_[internal::_pack_value(triple.head, triple.relation)].insert(triple.tail);
    }
}


/* sample size: (len(batch_size), positive + negatives, 3) */
void LCWANoThrowSampler::HashSampleStrategy::sample(py::array_t<int32_t, py::array::c_style | py::array::forcecast>& arr, const py::list& batch, int64_t random_seed)
{
    /* IMPROVE: use determined seed may help with reproducible result. Yet here we are using random seed */
    std::mt19937_64 random_engine(random_seed);
    /* https://codereview.stackexchange.com/posts/109518/revisions "Improved long-period generators based on linear recurrences modulo 2", F. Panneton, P. L'Ecuyer, M. Matsumoto in AVM TOMS Volume 32 Issue 1, March 2006 Pages 1-16 */
    random_engine.discard(700000);
    auto corrupt_head_list = batch[0].cast<py::list>();
    auto samples = batch[1].cast<py::list>();
    auto tensor = arr.mutable_unchecked<3>(); // Will throw if ndim != 3 or flags.writeable is false
    for (ssize_t i = 0; i < tensor.shape(0); i++) {
        auto triple = samples[i].cast<TripleIndex>();
        auto h = triple.head;
        auto r = triple.relation;
        auto t = triple.tail;
        auto corrupt_head = corrupt_head_list[i].cast<bool>();

        /*
        * first sample is positive sample
        */
        tensor(i, 0, 0) = h;
        tensor(i, 0, 1) = r;
        tensor(i, 0, 2) = t;

        /* negative samples */
        std::function<int32_t(void)> gen_func = [&]() -> int16_t { return random_engine() % max_entity_; };
        constexpr auto kNumNegativeEntityIndexOffset = 1;
        for (ssize_t j = kNumNegativeEntityIndexOffset; j < sampler_->num_negative_entity_ + kNumNegativeEntityIndexOffset; j++) {
            for (ssize_t k = 0; k < tensor.shape(2); k++) {
                if (corrupt_head) {
                    tensor(i, j, 0) = h;
                    tensor(i, j, 2) = generateCorruptHead(h, r, gen_func);
                } else {
                    tensor(i, j, 0) = generateCorruptTail(t, r, gen_func);
                    tensor(i, j, 2) = t;
                }
                tensor(i, j, 1) = r;
            }
        }
    }
}

int32_t LCWANoThrowSampler::HashSampleStrategy::generateCorruptHead(int32_t h, int32_t r, std::function<int32_t(void)> generate_random_func)
{
    auto k = internal::_pack_value(h, r);
    auto gen_tail = generate_random_func();
    if (rest_tail_[k].find(gen_tail) == rest_tail_[k].end()) {
        return gen_tail;
    } else {
        for (auto i = 0; i < max_entity_; ++i) {
            gen_tail = (gen_tail + 1) % max_entity_;
            if (rest_tail_[k].find(gen_tail) == rest_tail_[k].end()) {
                return gen_tail;
            }
        }
        throw std::runtime_error("A triple seems to match all entities. It's almost impossible");
    }
}

int32_t LCWANoThrowSampler::HashSampleStrategy::generateCorruptTail(int32_t t, int32_t r, std::function<int32_t(void)> generate_random_func)
{
    auto k = internal::_pack_value(t, r);
    auto gen_head = generate_random_func();
    if (rest_head_[k].find(gen_head) == rest_head_[k].end()) {
        return gen_head;
    } else {
        for (auto i = 0; i < max_entity_; ++i) {
            gen_head = (gen_head + 1) % max_entity_;
            if (rest_head_[k].find(gen_head) == rest_head_[k].end()) {
                return gen_head;
            }
        }
        throw std::runtime_error("A triple seems to match all entities. It's almost impossible");
    }
}


} // namespace kgekit
