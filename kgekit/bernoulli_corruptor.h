#pragma once

#include <utility>
#include <unordered_map>
#include <vector>

#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <boost/core/noncopyable.hpp>

namespace kgekit {

using std::vector;
using std::pair;
using std::make_pair;

namespace py = pybind11;

class BernoulliCorruptor: private boost::noncopyable {
public:
    BernoulliCorruptor(const py::list& train_set);
    /*
    * tph, hpt are considered per relation basis.
    */
   float getProbablityRelation(const int32_t relation_id);
private:
    py::list train_set_;
    vector<float> average_tails_per_head_;
    vector<float> average_heads_per_tail_;
    int32_t num_relations_ = -1;
};

} // namespace kgekit
