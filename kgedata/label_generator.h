#pragma once

#include <cstdint>
#include <unordered_set>

#include <pybind11/numpy.h>
#include <boost/core/noncopyable.hpp>

#include "common.h"

namespace kgedata {

using std::unordered_set;

namespace py = pybind11;

namespace detail {

struct TripleIndexHasher
{
  size_t operator()(const TripleIndex& obj) const
  {
    return std::hash<std::string>()(obj.repr());
  }
};

}  // namespace detail

class LabelGenerator : private boost::noncopyable
{
 public:
  explicit LabelGenerator(const py::array_t<int64_t>& triple_set);
  py::array_t<float, py::array::c_style> generate_labels(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
      float true_label = 1.0, float false_label = -1.0);

 private:
  unordered_set<TripleIndex, detail::TripleIndexHasher> triples_;
};

class StaticLabelGenerator : private boost::noncopyable
{
 public:
  explicit StaticLabelGenerator(bool true_label = false);
  py::array_t<float, py::array::c_style> generate_labels(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
      float true_label = 1.0, float false_label = -1.0);

 private:
  bool true_label_;
};

}  // namespace kgedata
