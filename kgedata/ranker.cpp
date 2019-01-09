#include "ranker.h"

#include <cmath>

namespace kgedata {

pair<int32_t, int32_t> _rank(pybind11::array_t<float>& arr, int64_t original, unordered_set<int64_t>& filters);

Ranker::Ranker(const pybind11::list& train_triples, const pybind11::list& valid_triples, const pybind11::list& test_triples)
{
    for (auto const& t : train_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[detail::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[detail::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[detail::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }

    for (auto const& t : valid_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[detail::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[detail::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[detail::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }

    for (auto const& t : test_triples) {
        auto triple = t.cast<TripleIndex>();
        rest_head_[detail::_pack_value(triple.relation, triple.tail)].insert(triple.head);
        rest_tail_[detail::_pack_value(triple.head, triple.relation)].insert(triple.tail);
        rest_relation_[detail::_pack_value(triple.head, triple.tail)].insert(triple.relation);
    }
}

Ranker::Ranker(const pybind11::tuple& rest_head, const pybind11::tuple& rest_tail, const pybind11::tuple& rest_relation)
{
    auto rest_head_keys = rest_head[0].cast<pybind11::list>();
    auto rest_head_sets = rest_head[1].cast<pybind11::list>();
    auto i = 0;
    for (const auto& key : rest_head_keys) {
        for (const auto& item : rest_head_sets[i]) {
            rest_head_[key.cast<int64_t>()].insert(item.cast<int64_t>());
        }
        i++;
    }
    i = 0;
    auto rest_tail_keys = rest_tail[0].cast<pybind11::list>();
    auto rest_tail_sets = rest_tail[1].cast<pybind11::list>();
    for (const auto& key : rest_tail_keys) {
        for (const auto& item : rest_tail_sets[i]) {
            rest_tail_[key.cast<int64_t>()].insert(item.cast<int64_t>());
        }
        i++;
    }
    i = 0;
    auto rest_relation_keys = rest_relation[0].cast<pybind11::list>();
    auto rest_relation_sets = rest_relation[1].cast<pybind11::list>();
    for (const auto& key : rest_relation_keys) {
        for (const auto& item : rest_relation_sets[i]) {
            rest_relation_[key.cast<int64_t>()].insert(item.cast<int64_t>());
        }
        i++;
    }
}

/* Not using index based creation cause segfault */
pybind11::tuple Ranker::exportState() const
{
    pybind11::list rest_head_keys(rest_head_.size());
    pybind11::list rest_head_sets(rest_head_.size());
    auto i = 0;
    auto j = 0;
    for (const auto& it: rest_head_) {
        int64_t rest_key = it.first;
        auto entry_set = it.second;

        rest_head_keys[i] = rest_key;
        pybind11::list entry_container(entry_set.size());
        j = 0;
        for (int64_t item : entry_set) {
            entry_container[j++] = item;
        }
        rest_head_sets[i++] = entry_container;
    }
    if (rest_head_keys.size() != rest_head_sets.size()) {
        throw std::runtime_error("Invalid head k/v mismatch.");
    }
    pybind11::tuple rest_head = pybind11::make_tuple(rest_head_keys, rest_head_sets);

    pybind11::list rest_tail_keys(rest_tail_.size());
    pybind11::list rest_tail_sets(rest_tail_.size());
    i = 0;
    for (const auto& it: rest_tail_) {
        int64_t rest_key = it.first;
        auto entry_set = it.second;

        rest_tail_keys[i] = rest_key;
        pybind11::list entry_container(entry_set.size());
        j = 0;
        for (int64_t item : entry_set) {
            entry_container[j++] = item;
        }
        rest_tail_sets[i++] = entry_container;
    }
    if (rest_tail_keys.size() != rest_tail_sets.size()) {
        throw std::runtime_error("Invalid head k/v mismatch.");
    }
    pybind11::tuple rest_tail = pybind11::make_tuple(rest_tail_keys, rest_tail_sets);

    pybind11::list rest_relation_keys(rest_relation_.size());
    pybind11::list rest_relation_sets(rest_relation_.size());
    i = 0;
    for (const auto& it: rest_relation_) {
        int64_t rest_key = it.first;
        auto entry_set = it.second;

        rest_relation_keys[i] = rest_key;
        pybind11::list entry_container(entry_set.size());
        j = 0;
        for (int64_t item : entry_set) {
            entry_container[j++] = item;
        }
        rest_relation_sets[i++] = entry_container;
    }
    if (rest_relation_keys.size() != rest_relation_sets.size()) {
        throw std::runtime_error("Invalid head k/v mismatch.");
    }
    pybind11::tuple rest_relation = pybind11::make_tuple(rest_relation_keys, rest_relation_sets);

    return pybind11::make_tuple(rest_head, rest_tail, rest_relation);
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
    return _rank(arr, triple.head, rest_head_[detail::_pack_value(triple.relation, triple.tail)]);
}

pair<int32_t, int32_t> Ranker::rankTail(pybind11::array_t<float>& arr, const TripleIndex& triple)
{
    return _rank(arr, triple.tail, rest_tail_[detail::_pack_value(triple.head, triple.relation)]);
}

pair<int32_t, int32_t> Ranker::rankRelation(pybind11::array_t<float>& arr, const TripleIndex& triple)
{
    return _rank(arr, triple.relation, rest_relation_[detail::_pack_value(triple.head, triple.tail)]);
}

pybind11::tuple ranker_pickle_getstate(const Ranker& ranker)
{
    return ranker.exportState();
}

Ranker ranker_pickle_setstate(pybind11::tuple t)
{
    if (t.size() != 3) {
        throw std::runtime_error("Invalid state!");
    }
    auto rest_head = t[0].cast<pybind11::tuple>();
    if (rest_head[0].cast<pybind11::list>().size() != rest_head[1].cast<pybind11::list>().size()) {
        throw std::runtime_error("Invalid state!");
    }
    auto rest_tail = t[1].cast<pybind11::tuple>();
    if (rest_tail[0].cast<pybind11::list>().size() != rest_tail[1].cast<pybind11::list>().size()) {
        throw std::runtime_error("Invalid state!");
    }
    auto rest_relation = t[2].cast<pybind11::tuple>();
    if (rest_relation[0].cast<pybind11::list>().size() != rest_relation[1].cast<pybind11::list>().size()) {
        throw std::runtime_error("Invalid state!");
    }

    return Ranker(rest_head,  rest_tail, rest_relation);
}


} // namespace kgedata
