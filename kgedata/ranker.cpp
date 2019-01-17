#include "ranker.h"

#include <cmath>

namespace kgedata {

pair<int32_t, int32_t> _rank(py::array_t<float>& arr, int64_t original,
                             unordered_set<int64_t>& filters);

Ranker::Ranker(const py::array_t<int64_t>& train_triples,
               const py::array_t<int64_t>& valid_triples,
               const py::array_t<int64_t>& test_triples)
{
  {
    auto arr = train_triples.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
      auto head = arr(i, detail::kTripleHeadOffestInABatch);
      auto relation = arr(i, detail::kTripleRelationOffestInABatch);
      auto tail = arr(i, detail::kTripleTailOffestInABatch);
      rest_head_[detail::_pack_value(relation, tail)].insert(head);
      rest_relation_[detail::_pack_value(head, tail)].insert(relation);
      rest_tail_[detail::_pack_value(head, relation)].insert(tail);
    }
  }

  {
    auto arr = valid_triples.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
      auto head = arr(i, detail::kTripleHeadOffestInABatch);
      auto relation = arr(i, detail::kTripleRelationOffestInABatch);
      auto tail = arr(i, detail::kTripleTailOffestInABatch);
      rest_head_[detail::_pack_value(relation, tail)].insert(head);
      rest_relation_[detail::_pack_value(head, tail)].insert(relation);
      rest_tail_[detail::_pack_value(head, relation)].insert(tail);
    }
  }

  {
    auto arr = test_triples.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
      auto head = arr(i, detail::kTripleHeadOffestInABatch);
      auto relation = arr(i, detail::kTripleRelationOffestInABatch);
      auto tail = arr(i, detail::kTripleTailOffestInABatch);
      rest_head_[detail::_pack_value(relation, tail)].insert(head);
      rest_relation_[detail::_pack_value(head, tail)].insert(relation);
      rest_tail_[detail::_pack_value(head, relation)].insert(tail);
    }
  }
}

Ranker::Ranker(const py::tuple& rest_head, const py::tuple& rest_tail,
               const py::tuple& rest_relation)
{
  auto rest_head_keys = rest_head[0].cast<py::list>();
  auto rest_head_sets = rest_head[1].cast<py::list>();
  auto i = 0;
  for (const auto& key : rest_head_keys) {
    for (const auto& item : rest_head_sets[i]) {
      rest_head_[key.cast<int64_t>()].insert(item.cast<int64_t>());
    }
    i++;
  }
  i = 0;
  auto rest_tail_keys = rest_tail[0].cast<py::list>();
  auto rest_tail_sets = rest_tail[1].cast<py::list>();
  for (const auto& key : rest_tail_keys) {
    for (const auto& item : rest_tail_sets[i]) {
      rest_tail_[key.cast<int64_t>()].insert(item.cast<int64_t>());
    }
    i++;
  }
  i = 0;
  auto rest_relation_keys = rest_relation[0].cast<py::list>();
  auto rest_relation_sets = rest_relation[1].cast<py::list>();
  for (const auto& key : rest_relation_keys) {
    for (const auto& item : rest_relation_sets[i]) {
      rest_relation_[key.cast<int64_t>()].insert(item.cast<int64_t>());
    }
    i++;
  }
}

/* Not using index based creation cause segfault */
py::tuple Ranker::export_state() const
{
  py::list rest_head_keys(rest_head_.size());
  py::list rest_head_sets(rest_head_.size());
  auto i = 0;
  auto j = 0;
  for (const auto& it : rest_head_) {
    int64_t rest_key = it.first;
    auto entry_set = it.second;

    rest_head_keys[i] = rest_key;
    py::list entry_container(entry_set.size());
    j = 0;
    for (int64_t item : entry_set) {
      entry_container[j++] = item;
    }
    rest_head_sets[i++] = entry_container;
  }
  if (rest_head_keys.size() != rest_head_sets.size()) {
    throw std::runtime_error("Invalid head k/v mismatch.");
  }
  py::tuple rest_head = py::make_tuple(rest_head_keys, rest_head_sets);

  py::list rest_tail_keys(rest_tail_.size());
  py::list rest_tail_sets(rest_tail_.size());
  i = 0;
  for (const auto& it : rest_tail_) {
    int64_t rest_key = it.first;
    auto entry_set = it.second;

    rest_tail_keys[i] = rest_key;
    py::list entry_container(entry_set.size());
    j = 0;
    for (int64_t item : entry_set) {
      entry_container[j++] = item;
    }
    rest_tail_sets[i++] = entry_container;
  }
  if (rest_tail_keys.size() != rest_tail_sets.size()) {
    throw std::runtime_error("Invalid head k/v mismatch.");
  }
  py::tuple rest_tail = py::make_tuple(rest_tail_keys, rest_tail_sets);

  py::list rest_relation_keys(rest_relation_.size());
  py::list rest_relation_sets(rest_relation_.size());
  i = 0;
  for (const auto& it : rest_relation_) {
    int64_t rest_key = it.first;
    auto entry_set = it.second;

    rest_relation_keys[i] = rest_key;
    py::list entry_container(entry_set.size());
    j = 0;
    for (int64_t item : entry_set) {
      entry_container[j++] = item;
    }
    rest_relation_sets[i++] = entry_container;
  }
  if (rest_relation_keys.size() != rest_relation_sets.size()) {
    throw std::runtime_error("Invalid head k/v mismatch.");
  }
  py::tuple rest_relation =
      py::make_tuple(rest_relation_keys, rest_relation_sets);

  return py::make_tuple(rest_head, rest_tail, rest_relation);
}

template <typename T, typename compare=std::less<T>>
inline bool _rank_precedence(const T& lhs, const T& rhs)
{
  return compare()(lhs, rhs);
}

pair<int32_t, int32_t> _rank(py::array_t<float>& arr, int64_t original,
                             unordered_set<int64_t>& filters,
                             bool rank_higher)
{
  constexpr auto kEpsilon = 1e-6;
  auto p = static_cast<float*>(arr.request().ptr);
  float expected_best = p[original];
  // rank starts with 1
  int32_t rank = 1;
  int32_t filtered_rank = 1;
  auto op = rank_higher ? &_rank_precedence<float, std::greater<float>> :
    &_rank_precedence<float, std::less<float>>;
  for (ssize_t i = 0; i < arr.shape(0); ++i) {
    if (fabs(p[i] - expected_best) < kEpsilon) {
      continue;
    }
    else if (op(p[i], expected_best)) {
      rank++;
      if (filters.find(i) == filters.end()) {
        filtered_rank++;
      }
    }
  }
  return make_pair(rank, filtered_rank);
}

pair<int32_t, int32_t> Ranker::rank_head(py::array_t<float>& arr,
                                         py::array_t<int64_t>& triple,
                                         bool rank_higher)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(
      arr, p[detail::kTripleHeadOffestInABatch],
      rest_head_[detail::_pack_value(p[detail::kTripleRelationOffestInABatch],
                                     p[detail::kTripleTailOffestInABatch])],
      rank_higher);
}

pair<int32_t, int32_t> Ranker::rank_tail(py::array_t<float>& arr,
                                         py::array_t<int64_t>& triple,
                                         bool rank_higher)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(arr, p[detail::kTripleTailOffestInABatch],
               rest_tail_[detail::_pack_value(
                   p[detail::kTripleHeadOffestInABatch],
                   p[detail::kTripleRelationOffestInABatch])],
                   rank_higher);
}

pair<int32_t, int32_t> Ranker::rank_relation(py::array_t<float>& arr,
                                             py::array_t<int64_t>& triple,
                                             bool rank_higher)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(arr, p[detail::kTripleRelationOffestInABatch],
               rest_relation_[detail::_pack_value(
                   p[detail::kTripleHeadOffestInABatch],
                   p[detail::kTripleTailOffestInABatch])],
                   rank_higher);
}

py::tuple ranker_pickle_getstate(const Ranker& ranker)
{
  return ranker.export_state();
}

Ranker ranker_pickle_setstate(py::tuple t)
{
  if (t.size() != 3) {
    throw std::runtime_error("Invalid state!");
  }
  auto rest_head = t[0].cast<py::tuple>();
  if (rest_head[0].cast<py::list>().size() !=
      rest_head[1].cast<py::list>().size()) {
    throw std::runtime_error("Invalid state!");
  }
  auto rest_tail = t[1].cast<py::tuple>();
  if (rest_tail[0].cast<py::list>().size() !=
      rest_tail[1].cast<py::list>().size()) {
    throw std::runtime_error("Invalid state!");
  }
  auto rest_relation = t[2].cast<py::tuple>();
  if (rest_relation[0].cast<py::list>().size() !=
      rest_relation[1].cast<py::list>().size()) {
    throw std::runtime_error("Invalid state!");
  }

  return Ranker(rest_head, rest_tail, rest_relation);
}

}  // namespace kgedata
