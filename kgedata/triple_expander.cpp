#include "triple_expander.h"

namespace kgedata {

namespace py = pybind11;

pair<py::array_t<int64_t>, py::list> expand_triple_batch(
    const py::list& batch,
    int64_t num_entity,
    int64_t num_relation,
    bool expand_entity,
    bool expand_relation)
{
    int64_t num_expands = 0;
    if (expand_entity) {
        num_expands += num_entity * 2;
    } else {
        num_entity = 0;
    }
    if (expand_relation) {
        num_expands += num_relation;
    } else {
        num_relation = 0;
    }

    py::array_t<int64_t, py::array::c_style> arr({static_cast<py::ssize_t>(batch.size() * num_expands), static_cast<py::ssize_t>(3)});
    auto r = arr.mutable_unchecked<2>();
    auto base_offset = 0;
    py::list split_points;
    for (auto const& t : batch) {
        auto triple_index = t.cast<TripleIndex>();
        py::tuple splits(4);
        splits[0] = base_offset;
        /* link prediction on head */
        for (auto i_head = 0; i_head < num_entity; ++i_head) {
            r(base_offset + i_head, 0) = i_head;
            r(base_offset + i_head, 1) = triple_index.relation;
            r(base_offset + i_head, 2) = triple_index.tail;
        }
        base_offset += num_entity;
        splits[1] = base_offset;
        /* link prediction on tail */
        for (auto i_tail = 0; i_tail < num_entity; ++i_tail) {
            r(base_offset + i_tail, 0) = triple_index.head;
            r(base_offset + i_tail, 1) = triple_index.relation;
            r(base_offset + i_tail, 2) = i_tail;
        }
        base_offset += num_entity;
        splits[2] = base_offset;
        /* link prediction on relation */
        for (auto i_relation = 0; i_relation < num_relation; ++i_relation) {
            r(base_offset + i_relation, 0) = triple_index.head;
            r(base_offset + i_relation, 1) = i_relation;
            r(base_offset + i_relation, 2) = triple_index.tail;
        }
        base_offset += num_relation;
        splits[3] = base_offset;
        split_points.append(splits);
    }
    return make_pair(arr, split_points);
}

}; // namespace kgedata
