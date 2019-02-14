#include "label_generator.h"

#include <functional>

namespace kgedata {

using std::function;
using std::vector;

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
}

StaticLabelGenerator::StaticLabelGenerator(bool true_label)
    : true_label_(true_label)
{}

inline py::array_t<float, py::array::c_style> _gen_label_from_shape(
    vector<ssize_t>& shape,
    const int64_t* batch,
    function<void(ssize_t, const int64_t*, float*)> gen_labels)
{
  // the number of triple elements is in the last dimension.
  const auto num_triples = std::accumulate(
      std::next(shape.rbegin()), shape.rend(), 1, std::multiplies<ssize_t>());
  float* data = new float[num_triples];

  gen_labels(num_triples, batch, data);

  py::capsule free_when_done(data, [](void* f) {
    float* data = reinterpret_cast<float*>(f);
    delete[] data;
  });

  shape.pop_back();
  return py::array_t<float, py::array::c_style>(
      shape,
      data,             // the data pointer
      free_when_done);  // numpy array references this parent
}

inline py::array_t<float, py::array::c_style> _gen_label(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
    function<void(ssize_t, const int64_t*, float*)> gen_labels)
{
  auto shape = batch.request().shape;
  auto p = const_cast<const int64_t*>(static_cast<int64_t*>(batch.request().ptr));

  return _gen_label_from_shape(shape, p, gen_labels);
}

py::array_t<float, py::array::c_style> LabelGenerator::generate_labels(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
    float true_label, float false_label)
{
  return _gen_label(batch, [true_label, false_label, this](ssize_t num_triples,
                                                           const int64_t* batch,
                                                           float* data) {
    for (size_t i = 0; i < num_triples; i++) {
      auto base_adr = batch + i * detail::kNumTripleElements;
      auto h = *base_adr;
      auto r = *(base_adr + detail::kTripleRelationOffestInABatch);
      auto t = *(base_adr + detail::kTripleTailOffestInABatch);
      data[i] =
          (triples_.find(TripleIndex(h, r, t)) == triples_.end() ? false_label
                                                                 : true_label);
    }
  });
}

py::array_t<float, py::array::c_style> StaticLabelGenerator::generate_labels(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch,
    float true_label, float false_label)
{
  return _gen_label(
      batch, [true_label, false_label, this](ssize_t num_triples,
                                             const int64_t* batch, float* data) {
        for (size_t i = 0; i < num_triples; i++) {
          data[i] = true_label_ ? true_label : false_label;
        }
      });
}

py::array_t<float, py::array::c_style> StaticLabelGenerator::generate_labels(
    py::tuple shape, float true_label, float false_label)
{
  vector<ssize_t> shape_vec;
  for (auto const& v : shape) {
    auto val = v.cast<ssize_t>();
    shape_vec.push_back(val);
  }
  return _gen_label_from_shape(
      shape_vec, nullptr, [true_label, false_label, this](ssize_t num_triples,
                                             const int64_t* batch, float* data) {
        for (size_t i = 0; i < num_triples; i++) {
          data[i] = true_label_ ? true_label : false_label;
        }
      });
}
}  // namespace kgedata
