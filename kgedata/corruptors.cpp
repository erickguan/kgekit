#include "corruptors.h"

#include <algorithm>
#include <unordered_set>

#include "common.h"

namespace kgedata {

using std::unordered_set;

static const int kDefaultProbability = 50;
static const int kNoRelation = 0;
static const float kEqualProbability = 0.5;

BernoulliCorruptor::BernoulliCorruptor(const py::list& train_set, int32_t num_relations, int64_t random_seed)
    : Corruptor(random_seed), num_relations_(num_relations)
{
    distributions_.reserve(num_relations_);
    average_tails_per_head_.resize(num_relations_, 0.0);
    average_heads_per_tail_.resize(num_relations_, 0.0);
    vector<int32_t> triples_per_rel(num_relations_, 0);
    vector<unordered_set<int32_t>> tails_per_rel(num_relations_);
    vector<unordered_set<int32_t>> heads_per_rel(num_relations_);

    for (auto const& t : train_set) {
        auto triple = t.cast<TripleIndex>();
        triples_per_rel[triple.relation]++;
        tails_per_rel[triple.relation].insert(triple.tail);
        heads_per_rel[triple.relation].insert(triple.head);
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

UniformCorruptor::UniformCorruptor(int64_t random_seed)
    : Corruptor(random_seed)
{
}

void BernoulliCorruptor::make_random_choice(const py::list& batch, py::array_t<bool, py::array::c_style | py::array::forcecast>& arr)
{
    auto r = arr.mutable_unchecked<1>();
    auto i = 0;
    for (const auto& e : batch) {
        auto ele = e.cast<TripleIndex>();
        r(i) = distributions_[ele.relation](random_engine_) == 0;
        i++;
    }
}


void UniformCorruptor::make_random_choice(const py::list& batch, py::array_t<bool, py::array::c_style | py::array::forcecast>& arr)
{
    discrete_distribution<> d({kDefaultProbability, kDefaultProbability});
    std::generate_n(arr.mutable_data(), batch.size(), [&]() { return d(random_engine_) == 0; });
}

} // namespace kgedata
