#include "corruptors.h"

#include <algorithm>
#include <unordered_set>

#include "common.h"

namespace kgedata {

using std::unordered_set;

static const int kDefaultProbability = 50;
static const int kNoRelation = 0;
static const float kEqualProbability = 0.5;

BernoulliCorruptor::BernoulliCorruptor(
    const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& train_set,
    int32_t num_relations,
    int64_t num_negative_entity,
    int64_t random_seed)
    : Corruptor(num_negative_entity, random_seed), num_relations_(num_relations)
{
    distributions_.reserve(num_relations_);
    average_tails_per_head_.resize(num_relations_, 0.0);
    average_heads_per_tail_.resize(num_relations_, 0.0);
    vector<int32_t> triples_per_rel(num_relations_, 0);
    vector<unordered_set<int32_t>> tails_per_rel(num_relations_);
    vector<unordered_set<int32_t>> heads_per_rel(num_relations_);

    auto arr = train_set.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
        auto head = arr(i, 0);
        auto relation = arr(i, 1);
        auto tail = arr(i, 2);
        triples_per_rel[relation]++;
        tails_per_rel[relation].insert(tail);
        heads_per_rel[relation].insert(head);
    }

    for (auto i = 0; i < num_relations_; ++i) {
        if (triples_per_rel[i] == kNoRelation ||
            tails_per_rel[i].size() == kNoRelation ||
            heads_per_rel[i].size() == kNoRelation) {
            average_tails_per_head_[i] = average_heads_per_tail_[i] = kEqualProbability;
            discrete_distribution d({kDefaultProbability, kDefaultProbability});
            distributions_.push_back(d);
        } else {
            average_tails_per_head_[i] = static_cast<float>(triples_per_rel[i]) / heads_per_rel[i].size();
            average_heads_per_tail_[i] = static_cast<float>(triples_per_rel[i]) / tails_per_rel[i].size();
            auto r = getProbabilityRelation(i);
            discrete_distribution d({r.first, r.second});
            distributions_.push_back(d);
        }
    }
}

pair<double, double> BernoulliCorruptor::getProbabilityRelation(const int32_t relation_id) const
{
    auto hpt = average_heads_per_tail_[relation_id];
    auto tph = average_tails_per_head_[relation_id];
    return { tph, hpt };
}

double BernoulliCorruptor::get_probability_relation(const int32_t relation_id) const
{
    if (relation_id < 0 || relation_id >= num_relations_) {
        throw std::out_of_range("Relation id given to the BernoulliCorruptor is larger than it parsed from datasets.");
    }
    float hpt;
    float tph;
    std::tie(hpt, tph) = getProbabilityRelation(relation_id);
    return tph / (hpt + tph);
}

UniformCorruptor::UniformCorruptor(int64_t num_negative_entity, int64_t random_seed)
    : Corruptor(num_negative_entity, random_seed)
{
}

py::array_t<bool, py::array::c_style | py::array::forcecast>
BernoulliCorruptor::make_random_choice(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    const auto num_batch = batch.shape(0);
    const size_t size = num_batch * num_negative_entity_;
    bool* data = new bool[size];
    auto p = static_cast<int64_t (*)[detail::kNumTripleElements]>(batch.request().ptr);
    for (size_t i = 0; i < num_batch; i++) {
        for (auto j = 0; j < num_negative_entity_; ++j) {
            auto rel = p[i][detail::kTripleRelationOffestInABatch];
            *(data + i*num_negative_entity_ + j) = distributions_[rel](random_engine_) == 0;
        }
    }

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(data, [](void* f) {
        bool* data = reinterpret_cast<bool*>(f);
        delete[] data;
    });

    return py::array_t<bool, py::array::c_style | py::array::forcecast>(
        {static_cast<ssize_t>(num_batch), static_cast<ssize_t>(num_negative_entity_)}, // shape
        data, // the data pointer
        free_when_done); // numpy array references this parent
}


py::array_t<bool, py::array::c_style | py::array::forcecast>
UniformCorruptor::make_random_choice(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    const auto num_batch = batch.shape(0);
    const size_t size = num_batch * num_negative_entity_;
    bool* data = new bool[size];

    discrete_distribution<> d({kDefaultProbability, kDefaultProbability});
    for (size_t i = 0; i < num_batch; i++) {
        for (auto j = 0; j < num_negative_entity_; ++j) {
            data[i*num_negative_entity_ + j] = d(random_engine_) == 0;
        }
    }

    py::capsule free_when_done(data, [](void* f) {
        bool* data = reinterpret_cast<bool*>(f);
        delete[] data;
    });

    return py::array_t<bool, py::array::c_style | py::array::forcecast>(
        {static_cast<ssize_t>(num_batch), static_cast<ssize_t>(num_negative_entity_)}, // shape
        data, // the data pointer
        free_when_done); // numpy array references this parent
}

} // namespace kgedata
