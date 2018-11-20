#include "ranker.h"

#include <cmath>

namespace kgekit {

pair<int32_t, int32_t> _rank(pybind11::array_t<float>& arr, int64_t original, unordered_set<int64_t>& filters);

Ranker::Ranker(const pybind11::list& train_triples, const pybind11::list& valid_triples, const pybind11::list& test_triples)
{
    for (auto const& t : train_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[internal::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[internal::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[internal::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }

    for (auto const& t : valid_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[internal::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[internal::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[internal::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }

    for (auto const& t : test_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[internal::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[internal::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[internal::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }
}

pair<int32_t, int32_t> _rank(pybind11::array_t<float>& arr, int64_t original, unordered_set<int64_t>& filters)
{
    constexpr auto kEpsilon = 1e-6;
    auto r = arr.mutable_unchecked<1>(); // Will throw if ndim != 1 or flags.writeable is false
    float expected_best = r(original);
     // rank starts with 1
    int32_t rank = 1;
    int32_t filtered_rank = 1;
    for (ssize_t i = 0; i < r.shape(0); ++i) {
        if (fabs(r(i) - expected_best) < kEpsilon) {
            continue;
        } else if (r(i) < expected_best) {
            rank++;
            if (filters.find(i) == filters.end()) {
                filtered_rank++;
            }
        }
    }
    return make_pair(rank, filtered_rank);
}

pair<int32_t, int32_t> Ranker::rankHead(pybind11::array_t<float>& arr, const TripleIndex& triple)
{
    return _rank(arr, triple.head, rest_head_[internal::_pack_value(triple.relation, triple.tail)]);
}

pair<int32_t, int32_t> Ranker::rankTail(pybind11::array_t<float>& arr, const TripleIndex& triple)
{
    return _rank(arr, triple.tail, rest_tail_[internal::_pack_value(triple.head, triple.relation)]);
}

pair<int32_t, int32_t> Ranker::rankRelation(pybind11::array_t<float>& arr, const TripleIndex& triple)
{
    return _rank(arr, triple.relation, rest_relation_[internal::_pack_value(triple.head, triple.tail)]);
}

} // namespace kgekit
