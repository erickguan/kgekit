#pragma once

#include <utility>

#include <pybind11/pytypes.h>
#include <pybind11/numpy.h>

#include "kgedata.h"

namespace kgedata {

using std::pair;
using std::make_pair;

pair<pybind11::array_t<int64_t>, pybind11::list> expand_triple_batch(
    const pybind11::list& batch,
    int64_t num_entity,
    int64_t num_relation,
    bool expand_entity,
    bool expand_relation);

} // namespace kgedata
