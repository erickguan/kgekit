#include "triple_expander.h"

namespace kgedata {

pair<py::array_t<int64_t, py::array::c_style>, py::list> expand_triple_batch(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
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

    ssize_t batch_size = batch.shape(0) * num_expands;
    const size_t size = batch_size * detail::kNumTripleElements;
    int64_t* data = new int64_t[size];
    auto p = static_cast<int64_t*>(batch.request().ptr);
    py::list split_points;

    auto base_offset = 0;
    auto base_adr = data;
    for (auto i = 0; i < batch.shape(0); ++i) {
        py::tuple splits(4);
        auto adr = p + i*detail::kNumTripleElements;
        auto head = *adr;
        auto relation = *(adr + detail::kTripleRelationOffestInABatch);
        auto tail = *(adr + detail::kTripleTailOffestInABatch);

        /* link prediction on head */
        splits[0] = base_offset;
        for (auto i = 0; i < num_entity; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = i;
            *(data_adr + detail::kTripleRelationOffestInABatch) = relation;
            *(data_adr + detail::kTripleTailOffestInABatch) = tail;
        }

        base_offset += num_entity;
        base_adr += num_entity*detail::kNumTripleElements;

        /* link prediction on tail */
        splits[1] = base_offset;
        for (auto i = 0; i < num_entity; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = head;
            *(data_adr + detail::kTripleRelationOffestInABatch) = relation;
            *(data_adr + detail::kTripleTailOffestInABatch) = i;
        }

        base_offset += num_entity;
        base_adr += num_entity*detail::kNumTripleElements;

        /* link prediction on relation */
        splits[2] = base_offset;
        for (auto i = 0; i < num_relation; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = head;
            *(data_adr + detail::kTripleRelationOffestInABatch) = i;
            *(data_adr + detail::kTripleTailOffestInABatch) = tail;
        }
        base_offset += num_relation;
        base_adr += num_relation*detail::kNumTripleElements;
        splits[3] = base_offset;
        split_points.append(splits);
    }

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });

    auto arr = py::array_t<int64_t, py::array::c_style>(
        {batch_size, static_cast<ssize_t>(detail::kNumTripleElements)}, // shape
        data, // the data pointer
        free_when_done); // numpy array references this parent
    return make_pair(arr, split_points);
}

}; // namespace kgedata
