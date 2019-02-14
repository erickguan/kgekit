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

class LabelGenerator
{
 public:
  explicit LabelGenerator(float true_label_value = 1.0,
                          float false_label_value = -1.0)
      : true_label_value_(true_label_value),
        false_label_value_(false_label_value)
  {}

 protected:
  float true_label_value_;
  float false_label_value_;
};

class MemoryLabelGenerator : public LabelGenerator, private boost::noncopyable
{
 public:
  MemoryLabelGenerator(const py::array_t<int64_t>& triple_set,
                       float true_label_value = 1.0,
                       float false_label_value = -1.0);
  py::array_t<float, py::array::c_style> generate_labels(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);

 private:
  unordered_set<TripleIndex, detail::TripleIndexHasher> triples_;
};

class StaticLabelGenerator : public LabelGenerator, private boost::noncopyable
{
 public:
  StaticLabelGenerator(bool true_label = false, float true_label_value = 1.0,
                       float false_label_value = -1.0);
  py::array_t<float, py::array::c_style> generate_labels(
      py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);
  py::array_t<float, py::array::c_style> generate_labels(py::tuple shape);

 private:
  bool true_label_;
};

}  // namespace kgedata
