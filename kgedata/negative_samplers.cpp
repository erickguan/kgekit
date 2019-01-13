#include "negative_samplers.h"

#include <algorithm>

#include "kgedata.h"

namespace py = pybind11;

namespace kgedata {

PerturbationSampler::PerturbationSampler(
    const py::array_t<int64_t>& train_set,
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
        sample_strategy_ = make_unique<PerturbationSampler::HashSampleStrategy>(train_set, this);
    }
}

int16_t PerturbationSampler::numNegativeSamples() const
{
    return num_corrupt_entity_ + num_corrupt_relation_;
}

py::array_t<int64_t, py::array::c_style>
PerturbationSampler::sample(
    py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_arr,
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    return sample_strategy_->sample(corrupt_head_arr, batch);
}

PerturbationSampler::HashSampleStrategy::HashSampleStrategy(
    const py::array_t<int64_t>& triples,
    PerturbationSampler* sampler)
    : sampler_(sampler)
{
    auto arr = triples.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
        auto head = arr(i, detail::kTripleHeadOffestInABatch);
        auto relation = arr(i, detail::kTripleRelationOffestInABatch);
        auto tail = arr(i, detail::kTripleTailOffestInABatch);
        rest_head_[detail::_pack_value(relation, tail)].insert(head);
        rest_relation_[detail::_pack_value(head, tail)].insert(relation);
        rest_tail_[detail::_pack_value(head, relation)].insert(tail);
    }
}

/* sample size: (len(batch_size), negatives, 3) */
py::array_t<int64_t, py::array::c_style>
PerturbationSampler::HashSampleStrategy::sample(
    py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_flags,
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    const auto num_batch = batch.shape(0);
    auto num_corrupts = sampler_->num_corrupt_entity_+sampler_->num_corrupt_relation_;
    const size_t size = num_batch * num_corrupts * detail::kNumTripleElements;
    int64_t* data = new int64_t[size];

    auto corrupt_head_flags_proxy = static_cast<bool*>(corrupt_head_flags.request().ptr);
    auto batch_proxy = static_cast<int64_t*>(batch.request().ptr);

    for (ssize_t i = 0; i < num_batch; i++) {
        auto base_adr = batch_proxy + i*detail::kNumTripleElements;
        auto h = *base_adr;
        auto r = *(base_adr+detail::kTripleRelationOffestInABatch);
        auto t = *(base_adr+detail::kTripleTailOffestInABatch);

        /* negative samples */
        std::function<int64_t(void)> gen_func = [&]() -> int64_t { return sampler_->random_engine_() % sampler_->num_entity_; };
        ssize_t j = 0;
        auto corrupt_base_adr = corrupt_head_flags_proxy + i*sampler_->num_corrupt_entity_;
        auto batch_base_adr = data + i*num_corrupts*detail::kNumTripleElements;
        for (;
            j < sampler_->num_corrupt_entity_;
            ++j) {
            auto base_adr = batch_base_adr + j*detail::kNumTripleElements;
            if (*(corrupt_base_adr+j)) {
                *base_adr = h;
                *(base_adr+detail::kTripleTailOffestInABatch) = generateCorruptHead(h, r, gen_func);
            } else {
                *base_adr = generateCorruptTail(t, r, gen_func);
                *(base_adr+detail::kTripleTailOffestInABatch) = t;
            }
            *(base_adr+detail::kTripleRelationOffestInABatch) = r;
        }

        for (; j < num_corrupts; ++j) {
            auto base_adr = batch_base_adr + j*detail::kNumTripleElements;
            *base_adr = h;
            *(base_adr+detail::kTripleRelationOffestInABatch) = generateCorruptRelation(h, t, gen_func);
            *(base_adr+detail::kTripleTailOffestInABatch) = t;
        }
    }

    py::capsule free_when_done(data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });

    return py::array_t<int64_t, py::array::c_style>(
        {static_cast<ssize_t>(num_batch), static_cast<ssize_t>(num_corrupts), static_cast<ssize_t>(detail::kNumTripleElements)}, // shape
        data, // the data pointer
        free_when_done); // numpy array references this parent
}

int64_t PerturbationSampler::HashSampleStrategy::generateCorruptHead(int64_t h, int64_t r, const std::function<int64_t(void)>& generate_random_func)
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

int64_t PerturbationSampler::HashSampleStrategy::generateCorruptTail(int64_t t, int64_t r, const std::function<int64_t(void)>& generate_random_func)
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

int64_t PerturbationSampler::HashSampleStrategy::generateCorruptRelation(int64_t h, int64_t t, const std::function<int64_t(void)>& generate_random_func)
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

CWASampler::CWASampler(
        int64_t num_entity,
        int64_t num_relation,
        bool corrupt_relation)
    : num_entity_(num_entity), num_relation_(num_relation), corrupt_relation_(corrupt_relation)
{
}

py::array_t<int64_t, py::array::c_style> CWASampler::sample(
    py::array_t<bool, py::array::c_style | py::array::forcecast>& corrupt_head_flags,
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    const auto num_batch = batch.shape(0);
    auto num_corrupts = num_entity_ + (corrupt_relation_ ? num_relation_ : 0);
    int64_t* data = new int64_t[num_batch * num_corrupts * detail::kNumTripleElements];

    auto corrupt_head_flags_proxy = static_cast<bool*>(corrupt_head_flags.request().ptr);
    auto batch_proxy = static_cast<int64_t*>(batch.request().ptr);

    for (ssize_t i = 0; i < num_batch; i++) {
        auto base_adr = batch_proxy + i*detail::kNumTripleElements;
        int64_t h = *base_adr;
        int64_t r = *(base_adr+detail::kTripleRelationOffestInABatch);
        int64_t t = *(base_adr+detail::kTripleTailOffestInABatch);

        /* negative samples */
        auto corrupt_adr = corrupt_head_flags_proxy + i;
        auto batch_base_adr = data + i*num_corrupts*detail::kNumTripleElements;
        for (int64_t id = 0; id < num_entity_; ++id) {
            auto base_adr = batch_base_adr + id*detail::kNumTripleElements;
            if (*corrupt_adr) {
                *base_adr = id;
                *(base_adr+detail::kTripleRelationOffestInABatch) = r;
                *(base_adr+detail::kTripleTailOffestInABatch) = t;
            } else {
                *base_adr = h;
                *(base_adr+detail::kTripleRelationOffestInABatch) = r;
                *(base_adr+detail::kTripleTailOffestInABatch) = id;
            }
        }
        batch_base_adr += num_entity_*detail::kNumTripleElements;

        if (corrupt_relation_) {
            for (int64_t id = 0; id < num_relation_; ++id) {
                auto base_adr = batch_base_adr + id*detail::kNumTripleElements;
                *base_adr = h;
                *(base_adr+detail::kTripleRelationOffestInABatch) = id;
                *(base_adr+detail::kTripleTailOffestInABatch) = t;
            }
        }
    }

    py::capsule free_when_done(data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });

    return py::array_t<int64_t, py::array::c_style>(
        {static_cast<ssize_t>(num_batch), static_cast<ssize_t>(num_corrupts), static_cast<ssize_t>(detail::kNumTripleElements)}, // shape
        data, // the data pointer
        free_when_done);
}

} // namespace kgedata
