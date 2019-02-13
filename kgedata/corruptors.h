#pragma once

#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include <pybind11/numpy.h>
#include <pybind11/pytypes.h>
#include <boost/core/noncopyable.hpp>

#include "common.h"

namespace kgedata {

using std::discrete_distribution;
using std::make_pair;
using std::pair;
using std::vector;

namespace py = pybind11;

class Corruptor
{
 public:
  explicit Corruptor(int64_t num_negative_entity)
      : num_negative_entity_(num_negative_entity)
  {}

 protected:
  int64_t num_negative_entity_;
};

class RandomCorruptor
{
 public:
  explicit RandomCorruptor(int64_t random_seed) : random_engine_(random_seed)
  {
    /* https://codereview.stackexchange.com/posts/109518/revisions "Improved
     * long-period generators based on linear recurrences modulo 2", F.
     * Panneton, P. L'Ecuyer, M. Matsumoto in AVM TOMS Volume 32 Issue 1, March
     * 2006 Pages 1-16 */
    random_engine_.discard(700000);
  }

 protected:
  std::mt19937_64 random_engine_;
};

class BernoulliCorruptor : public Corruptor,
                           public RandomCorruptor,
                           private boost::noncopyable
{
 public:
  BernoulliCorruptor(const py::array_t<int64_t>& train_set,
                     int32_t num_relations, int64_t num_negative_entity,
                     int64_t random_seed = std::random_device{}());
  py::array_t<bool, py::array::c_style> make_random_choice(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);
  /*
   * tph, hpt are considered per relation basis.
   */
  double get_probability_relation(const int32_t relation_id) const;

 private:
  pair<double, double> getProbabilityRelation(const int32_t relation_id) const;
  vector<float> average_tails_per_head_;
  vector<float> average_heads_per_tail_;
  vector<discrete_distribution<>> distributions_;
  int32_t num_relations_ = -1;
};

class UniformCorruptor : public Corruptor,
                         public RandomCorruptor,
                         private boost::noncopyable
{
 public:
  UniformCorruptor(int64_t num_negative_entity,
                   int64_t random_seed = std::random_device{}());
  py::array_t<bool, py::array::c_style> make_random_choice(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);
};

class StaticCorruptor : public Corruptor, private boost::noncopyable
{
 public:
  StaticCorruptor(int64_t num_negative_entity, bool corrupt_head = true);
  py::array_t<bool, py::array::c_style> make_choice(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);

 private:
  bool corrupt_head_;
};

}  // namespace kgedata
