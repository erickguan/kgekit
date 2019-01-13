#include "label_generator.h"

namespace kgedata {

LabelGenerator::LabelGenerator(const py::array_t<int64_t>& triple_set)
{
    auto arr = triple_set.unchecked<2>();
    for (auto i = 0; i < arr.shape(0); ++i) {
        int64_t head = arr(i, detail::kTripleHeadOffestInABatch);
        int64_t relation = arr(i, detail::kTripleRelationOffestInABatch);
        int64_t tail = arr(i, detail::kTripleTailOffestInABatch);
        triples_.insert(TripleIndex(head, relation, tail));
    }
    unordered_set<TripleIndex, detail::TripleIndexHasher> triples_;
};


py::array_t<int64_t, py::array::c_style> LabelGenerator::generate_labels(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
    auto shape = batch.request().shape;
    // the number of triple elements is in the last dimension.
    const auto num_triples = std::accumulate(std::next(shape.rbegin()), shape.rend(), 1, std::multiplies<ssize_t>());
    int64_t* data = new int64_t[num_triples];
    auto p = static_cast<int64_t*>(batch.request().ptr);

    for (size_t i = 0; i < num_triples; i++) {
        auto base_adr = p + i*detail::kNumTripleElements;
        auto h = *base_adr;
        auto r = *(base_adr + detail::kTripleRelationOffestInABatch);
        auto t = *(base_adr + detail::kTripleTailOffestInABatch);
        data[i] = (triples_.find(TripleIndex(h, r, t)) == triples_.end() ? -1 : 1);
    }

    py::capsule free_when_done(data, [](void* f) {
        int64_t* data = reinterpret_cast<int64_t*>(f);
        delete[] data;
    });

    shape.pop_back();
    return py::array_t<int64_t, py::array::c_style>(
        shape,
        data, // the data pointer
        free_when_done); // numpy array references this parent
}

} // namespace kgedata