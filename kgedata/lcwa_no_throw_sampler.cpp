#include "lcwa_no_throw_sampler.h"

#include <algorithm>

#include "kgedata.h"

namespace py = pybind11;

namespace kgedata {

LCWANoThrowSampler::LCWANoThrowSampler(
    const py::list& train_set,
    int64_t num_entity,
    int64_t num_relation,
    int16_t num_corrupt_entity,
    int16_t num_corrupt_relation,
    int64_t random_seed,
    Strategy strategy)
    : num_entity_(num_entity),
    num_relation_(num_relation),
    num_corrupt_entity_(num_corrupt_entity),
    num_corrupt_relation_(num_corrupt_relation),
    random_engine_(random_seed)
{
    /* https://codereview.stackexchange.com/posts/109518/revisions "Improved long-period generators based on linear recurrences modulo 2", F. Panneton, P. L'Ecuyer, M. Matsumoto in AVM TOMS Volume 32 Issue 1, March 2006 Pages 1-16 */
    random_engine_.discard(700000);
    if (strategy == Strategy::Hash) {
        sample_strategy_ = make_unique<LCWANoThrowSampler::HashSampleStrategy>(train_set, this);
    }
}

int16_t LCWANoThrowSampler::numNegativeSamples() const
{
    return num_corrupt_entity_ + num_corrupt_relation_;
}

void LCWANoThrowSampler::sample(py::array_t<int64_t, py::array::c_style | py::array::forcecast>& arr,
    py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
    const py::list& batch)
{
    sample_strategy_->sample(arr, corrupt_head_arr, batch);
}

LCWANoThrowSampler::HashSampleStrategy::HashSampleStrategy(const py::list& triples, LCWANoThrowSampler* sampler)
    : sampler_(sampler)
{
    for (auto const& t : triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[detail::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_relation_[detail::_pack_value(triple.head, triple.tail)].insert(triple.relation);
        rest_tail_[detail::_pack_value(triple.head, triple.relation)].insert(triple.tail);
    }
}

/* sample size: (len(batch_size), negatives, 3) */
void LCWANoThrowSampler::HashSampleStrategy::sample(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& arr,
    py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
    const py::list& batch)
{
    auto tensor = arr.mutable_unchecked<3>(); // Will throw if ndim != 3 or flags.writeable is false
    auto corrupt_head = corrupt_head_arr.unchecked<1>();
    for (ssize_t i = 0; i < tensor.shape(0); i++) {
        auto triple = batch[i].cast<TripleIndex>();
        auto h = triple.head;
        auto r = triple.relation;
        auto t = triple.tail;
        auto corrupt_head_flag = corrupt_head[i];

        /* negative samples */
        std::function<int64_t(void)> gen_func = [&]() -> int64_t { return sampler_->random_engine_() % sampler_->num_entity_; };
        for (ssize_t j = 0;
            j < sampler_->num_corrupt_entity_;
            ++j) {
            if (corrupt_head_flag) {
                tensor(i, j, 0) = h;
                tensor(i, j, 2) = generateCorruptHead(h, r, gen_func);
            } else {
                tensor(i, j, 0) = generateCorruptTail(t, r, gen_func);
                tensor(i, j, 2) = t;
            }
            tensor(i, j, 1) = r;
        }
        auto num_corrupt_relation_index_offset = sampler_->num_corrupt_entity_;
        for (ssize_t j = num_corrupt_relation_index_offset;
            j < sampler_->num_corrupt_relation_ + num_corrupt_relation_index_offset;
            ++j) {
            tensor(i, j, 0) = h;
            tensor(i, j, 1) = generateCorruptRelation(h, t, gen_func);
            tensor(i, j, 2) = t;
        }
    }
}

int64_t LCWANoThrowSampler::HashSampleStrategy::generateCorruptHead(int64_t h, int64_t r, std::function<int64_t(void)> generate_random_func)
{
    auto k = detail::_pack_value(h, r);
    auto gen_tail = generate_random_func() % sampler_->num_entity_;
    if (rest_tail_[k].find(gen_tail) == rest_tail_[k].end()) {
        return gen_tail;
    } else {
        for (auto i = 0; i < sampler_->num_entity_; ++i) {
            gen_tail = (gen_tail + 1) % sampler_->num_entity_;
            if (rest_tail_[k].find(gen_tail) == rest_tail_[k].end()) {
                return gen_tail;
            }
        }
        throw std::runtime_error(fmt::format("Can't find a corrupt head ({0}, {1}, ?)", h, r ));
    }
}

int64_t LCWANoThrowSampler::HashSampleStrategy::generateCorruptTail(int64_t t, int64_t r, std::function<int64_t(void)> generate_random_func)
{
    auto k = detail::_pack_value(t, r);
    auto gen_head = generate_random_func() % sampler_->num_entity_;
    if (rest_head_[k].find(gen_head) == rest_head_[k].end()) {
        return gen_head;
    } else {
        for (auto i = 0; i < sampler_->num_entity_; ++i) {
            gen_head = (gen_head + 1) % sampler_->num_entity_;
            if (rest_head_[k].find(gen_head) == rest_head_[k].end()) {
                return gen_head;
            }
        }
        throw std::runtime_error(fmt::format("Can't find a corrupt head (?, {0}, {1})", r, t));
    }
}

int64_t LCWANoThrowSampler::HashSampleStrategy::generateCorruptRelation(int64_t h, int64_t t, std::function<int64_t(void)> generate_random_func)
{
    auto k = detail::_pack_value(h, t);
    auto gen_relation = generate_random_func() % sampler_->num_relation_;
    if (rest_relation_[k].find(gen_relation) == rest_relation_[k].end()) {
        return gen_relation;
    } else {
        for (auto i = 0; i < sampler_->num_relation_; ++i) {
            gen_relation = (gen_relation + 1) % sampler_->num_relation_;
            if (rest_relation_[k].find(gen_relation) == rest_relation_[k].end()) {
                return gen_relation;
            }
        }
        throw std::runtime_error(fmt::format("Can't find a corrupt relation ({0}, ?, {1}). Generated: {2}" , h, t, gen_relation));
    }
}

} // namespace kgedata
