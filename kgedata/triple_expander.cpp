#include "triple_expander.h"

namespace kgedata {

pair<py::array_t<int64_t, py::array::c_style>, py::array_t<int64_t, py::array::c_style>>
expand_triple_batch(
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

    ssize_t batch_size = batch.shape(0);
    ssize_t expansion_size = batch_size * num_expands;
    const size_t data_size = expansion_size * detail::kNumTripleElements;
    int64_t* data = new int64_t[data_size];
    auto p = static_cast<int64_t*>(batch.request().ptr);

    const size_t splits_size = batch_size * detail::kNumExpansionPoints;
    int64_t* splits_data = new int64_t[splits_size];

    auto base_offset = 0;
    auto base_adr = data;
    for (auto i = 0; i < batch_size; ++i) {
        auto adr = p + i*detail::kNumTripleElements;
        auto head = *adr;
        auto relation = *(adr + detail::kTripleRelationOffestInABatch);
        auto tail = *(adr + detail::kTripleTailOffestInABatch);
        auto split_adr = splits_data + i*detail::kNumExpansionPoints;

        /* link prediction on head */
        split_adr[detail::kExpansionHeadsOffest] = base_offset;
        for (auto i = 0; i < num_entity; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = i;
            *(data_adr + detail::kTripleRelationOffestInABatch) = relation;
            *(data_adr + detail::kTripleTailOffestInABatch) = tail;
        }

        base_offset += num_entity;
        base_adr += num_entity*detail::kNumTripleElements;

        /* link prediction on tail */
        split_adr[detail::kExpansionTailsOffest] = base_offset;
        for (auto i = 0; i < num_entity; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = head;
            *(data_adr + detail::kTripleRelationOffestInABatch) = relation;
            *(data_adr + detail::kTripleTailOffestInABatch) = i;
        }

        base_offset += num_entity;
        base_adr += num_entity*detail::kNumTripleElements;

        /* link prediction on relation */
        split_adr[detail::kExpansionRelationsOffest] = base_offset;
        for (auto i = 0; i < num_relation; ++i) {
            auto data_adr = base_adr + i*detail::kNumTripleElements;
            *data_adr = head;
            *(data_adr + detail::kTripleRelationOffestInABatch) = i;
            *(data_adr + detail::kTripleTailOffestInABatch) = tail;
        }
        base_offset += num_relation;
        base_adr += num_relation*detail::kNumTripleElements;
        split_adr[detail::kExpansionBatchOffest] = base_offset;
    }

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });
    py::capsule free_when_done_splits(splits_data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });

    auto arr = py::array_t<int64_t, py::array::c_style>(
        {expansion_size, static_cast<ssize_t>(detail::kNumTripleElements)}, // shape
        data, // the data pointer
        free_when_done); // numpy array references this parent
    auto split_points = py::array_t<int64_t, py::array::c_style>(
        {batch_size, static_cast<ssize_t>(detail::kNumExpansionPoints)}, // shape
        splits_data, // the data pointer
        free_when_done_splits); // numpy array references this parent
    return make_pair(arr, split_points);
}

}; // namespace kgedata
