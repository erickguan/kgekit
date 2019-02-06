#include "ranker.h"

#include <cmath>
#include <stdexcept>

namespace kgedata {

namespace detail {

constexpr auto kInitialCounterValue = 0;
constexpr auto kNumRankElements = 6;
constexpr auto kNumRankHeadOffset = 0;
constexpr auto kNumRankFilteredHeadOffset = 1;
constexpr auto kNumRankTailOffset = 2;
constexpr auto kNumRankFilteredTailOffset = 3;
constexpr auto kNumRankRelationOffset = 4;
constexpr auto kNumRankFilteredRelationOffset = 5;
constexpr auto kRankPlaceholder = -1;
constexpr auto kDefaultProducerConsumerQueueSize = 1024;

} // namespace detail

pair<int32_t, int32_t> _rank(py::array_t<float>& arr, int64_t original,
                             unordered_set<int64_t>& filters);

Ranker::Ranker(
    const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& train_triples,
    const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& valid_triples,
    const py::array_t<int64_t, py::array::c_style | py::array::forcecast>& test_triples)
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

optional<pair<int32_t, int32_t>> rank_raw(
  const float* begin,
  const float* end,
  int64_t original,
  unordered_set<int64_t>& filters,
  bool ascending_rank)
{
  if (begin == end) { return {}; }
  float expected_best = begin[original];
  // rank starts with 1
  int32_t rank = 1;
  int32_t filtered_rank = 1;
  auto op = ascending_rank ? &_rank_precedence<float, std::less<float>> :
    &_rank_precedence<float, std::greater<float>>;
  for (auto i = 0; begin != end; ++begin, ++i) {
    if (fabs(*begin - expected_best) < detail::kRankerEpsilon) {
      continue;
    } else if (op(*begin, expected_best)) {
      rank++;
      if (filters.find(i) == filters.end()) {
        filtered_rank++;
      }
    }
  }
  return make_pair(rank, filtered_rank);
}

pair<int32_t, int32_t> _rank(py::array_t<float>& arr, int64_t original,
                             unordered_set<int64_t>& filters,
                             bool ascending_rank)
{
  auto p = static_cast<float*>(arr.request().ptr);
  auto r = rank_raw(
    p,
    p + arr.shape(0),
    original,
    filters,
    ascending_rank
  );
  return *r;
}

pair<int32_t, int32_t> Ranker::rank_head(py::array_t<float>& arr,
                                         py::array_t<int64_t>& triple,
                                         bool ascending_rank)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(
      arr, p[detail::kTripleHeadOffestInABatch],
      rest_head_[detail::_pack_value(p[detail::kTripleRelationOffestInABatch],
                                     p[detail::kTripleTailOffestInABatch])],
      ascending_rank);
}

pair<int32_t, int32_t> Ranker::rank_tail(py::array_t<float>& arr,
                                         py::array_t<int64_t>& triple,
                                         bool ascending_rank)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(arr, p[detail::kTripleTailOffestInABatch],
               rest_tail_[detail::_pack_value(
                   p[detail::kTripleHeadOffestInABatch],
                   p[detail::kTripleRelationOffestInABatch])],
                   ascending_rank);
}

pair<int32_t, int32_t> Ranker::rank_relation(py::array_t<float>& arr,
                                             py::array_t<int64_t>& triple,
                                             bool ascending_rank)
{
  auto p = static_cast<int64_t*>(triple.request().ptr);
  return _rank(arr, p[detail::kTripleRelationOffestInABatch],
               rest_relation_[detail::_pack_value(
                   p[detail::kTripleHeadOffestInABatch],
                   p[detail::kTripleTailOffestInABatch])],
                   ascending_rank);
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

optional<pair<int32_t, int32_t>> Ranker::rank_head_raw(
    const float* begin,
    const float* end,
    const TripleIndex& triple,
    bool ascending_rank)
{
  return rank_raw(begin, end, triple.head,
      rest_head_[detail::_pack_value(triple.relation, triple.tail)],
      ascending_rank);
}

optional<pair<int32_t, int32_t>> Ranker::rank_tail_raw(
    const float* begin,
    const float* end,
    const TripleIndex& triple,
    bool ascending_rank)
{
  return rank_raw(begin, end, triple.tail,
    rest_tail_[detail::_pack_value(triple.head, triple.relation)],
    ascending_rank);
}

optional<pair<int32_t, int32_t>> Ranker::rank_relation_raw(
    const float* begin,
    const float* end,
    const TripleIndex& triple,
    bool ascending_rank)
{
  return rank_raw(begin, end, triple.relation,
    rest_relation_[detail::_pack_value(triple.head, triple.tail)],
    ascending_rank);
}

py::array_t<int64_t, py::array::c_style>
Ranker::submit(
    py::array_t<float, py::array::c_style | py::array::forcecast> prediction,
    py::array_t<int64_t, py::array::c_style | py::array::forcecast> triple,
    py::array_t<int64_t, py::array::c_style | py::array::forcecast> split_points,
    bool ascending_rank)
{
  auto prediction_data = static_cast<float*>(prediction.request().ptr);
  auto triple_data = static_cast<int64_t*>(triple.request().ptr);
  auto split_points_data = static_cast<int64_t*>(split_points.request().ptr);
  auto num_triple = triple.shape(0);

  int64_t* data = nullptr;

  {
    py::gil_scoped_release release;
    auto num_data = num_triple*detail::kNumRankElements;
    data = new int64_t[num_data];
    std::fill_n(data, num_data, detail::kRankPlaceholder);

    for (auto i = 0; i < num_triple; ++i) {
      auto splits_adr = split_points_data + i*detail::kNumExpansionPoints;
      auto splits_head = splits_adr[detail::kExpansionHeadsOffest];
      auto splits_tail = splits_adr[detail::kExpansionTailsOffest];
      auto splits_relation = splits_adr[detail::kExpansionRelationsOffest];
      auto splits_batch = splits_adr[detail::kExpansionBatchOffest];
      auto triple_adr = triple_data + i*detail::kNumTripleElements;
      auto data_adr = data + i*detail::kNumRankElements;
      TripleIndex triple(
        triple_adr[detail::kTripleHeadOffestInABatch],
        triple_adr[detail::kTripleRelationOffestInABatch],
        triple_adr[detail::kTripleTailOffestInABatch]);

      auto head_ranks = rank_head_raw(
        prediction_data + splits_head,
        prediction_data + splits_tail,
        triple,
        ascending_rank);

      auto tail_ranks = rank_tail_raw(
        prediction_data + splits_tail,
        prediction_data + splits_relation,
        triple,
        ascending_rank);
      auto relation_ranks = rank_relation_raw(
        prediction_data + splits_relation,
        prediction_data + splits_batch,
        triple,
        ascending_rank);

      if (auto unwrap = head_ranks) {
        data_adr[detail::kNumRankHeadOffset] = unwrap->first;
        data_adr[detail::kNumRankFilteredHeadOffset] = unwrap->second;
      }
      if (auto unwrap = tail_ranks) {
        data_adr[detail::kNumRankTailOffset] = unwrap->first;
        data_adr[detail::kNumRankFilteredTailOffset] = unwrap->second;
      }
      if (auto unwrap = relation_ranks) {
        data_adr[detail::kNumRankRelationOffset] = unwrap->first;
        data_adr[detail::kNumRankFilteredRelationOffset] = unwrap->second;
      }
    }
  }

  py::capsule free_when_done(data, [](void* f) {
    int64_t* data = reinterpret_cast<int64_t*>(f);
    delete[] data;
  });

  return py::array_t<int64_t, py::array::c_style>(
    {static_cast<ssize_t>(num_triple), static_cast<ssize_t>(detail::kNumRankElements)},
    data, // the data pointer
    free_when_done);
}

} // namespace kgedata
