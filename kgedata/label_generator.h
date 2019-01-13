#pragma once

#include <unordered_set>
#include <cstdint>

#include <pybind11/numpy.h>
#include <boost/core/noncopyable.hpp>

#include "common.h"

namespace kgedata {

using std::unordered_set;

namespace py = pybind11;

namespace detail {

struct TripleIndexHasher {
    size_t operator()(const TripleIndex& obj) const
    {
        return std::hash<std::string>()(obj.repr());
    }
};

} // namespace detail


class LabelGenerator : private boost::noncopyable {
public:
    LabelGenerator(const py::array_t<int64_t>& triple_set);
    py::array_t<float, py::array::c_style> generate_labels(
        py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch);
private:
    unordered_set<TripleIndex, detail::TripleIndexHasher> triples_;
};

} // namespace kgedata