#pragma once

#include <utility>

#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>

#include "kgedata.h"

namespace kgedata {

namespace py = pybind11;

using std::pair;
using std::make_pair;

pair<py::array_t<int64_t, py::array::c_style>, py::array_t<int64_t, py::array::c_style>>
expand_triple_batch(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
    int64_t num_entity,
    int64_t num_relation,
    bool expand_entity = true,
    bool expand_relation = false);

} // namespace kgedata
