#include "bernoulli_corruptor.h"

#include <unordered_set>

#include "kgekit.h"

namespace py = pybind11;

namespace kgekit {

using std::unordered_set;

BernoulliCorruptor::BernoulliCorruptor(const py::list& train_set)
    : train_set_(train_set)
{
    unordered_set<int32_t> relations;
    for (auto const& t : train_set) {
        auto triple = t.cast<TripleIndex>();
        relations.insert(triple.relation);
    }
    num_relations_ = relations.size();

    average_tails_per_head_.resize(num_relations_, 0.0);
    average_heads_per_tail_.resize(num_relations_, 0.0);
    vector<int32_t> triples_per_rel(num_relations_, 0);
    vector<unordered_set<int32_t>> tails_per_rel(num_relations_);
    vector<unordered_set<int32_t>> heads_per_rel(num_relations_);

    for (auto const& t : train_set_) {
        auto triple = t.cast<TripleIndex>();
        triples_per_rel[triple.relation]++;
        tails_per_rel[triple.relation].insert(triple.tail);
        heads_per_rel[triple.relation].insert(triple.head);
    }

    for (auto i = 0; i < num_relations_; ++i) {
        average_tails_per_head_[i] = static_cast<float>(triples_per_rel[i]) / heads_per_rel[i].size();
        average_heads_per_tail_[i] = static_cast<float>(triples_per_rel[i]) / tails_per_rel[i].size();
    }
}

float BernoulliCorruptor::getProbablityRelation(const int32_t relation_id)
{
    if (relation_id < 0 || relation_id >= num_relations_) {
        throw std::out_of_range("Relation id given to the BernoulliCorruptor is larger than it parsed from datasets.");
    }
    auto hpt = average_heads_per_tail_[relation_id];
    auto tph = average_tails_per_head_[relation_id];
    return tph / (hpt+tph);
}

} // namespace kgekit
