#include "label_generator.h"

#include <functional>

namespace kgedata {

using std::function;
using std::vector;

MemoryLabelGenerator::MemoryLabelGenerator(
    const py::array_t<int64_t>& triple_set, float true_label_value,
    float false_label_value)
    : LabelGenerator(true_label_value, false_label_value)
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

StaticLabelGenerator::StaticLabelGenerator(bool true_label,
                                           float true_label_value,
                                           float false_label_value)
    : LabelGenerator(true_label_value, false_label_value),
      true_label_(true_label)
{}

inline py::array_t<float, py::array::c_style> _gen_label_from_shape(
    vector<ssize_t>& shape, int64_t num_triples, const int64_t* batch,
    function<void(ssize_t, const int64_t*, float*)> gen_labels)
{
  float* data = new float[num_triples];

  gen_labels(num_triples, batch, data);

  py::capsule free_when_done(data, [](void* f) {
    float* data = reinterpret_cast<float*>(f);
    delete[] data;
  });

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
  // the number of triple elements is in the last dimension.
  const auto num_triples = std::accumulate(
      std::next(shape.rbegin()), shape.rend(), 1, std::multiplies<ssize_t>());
  shape.pop_back();
  auto p =
      const_cast<const int64_t*>(static_cast<int64_t*>(batch.request().ptr));

  return _gen_label_from_shape(shape, num_triples, p, gen_labels);
}

py::array_t<float, py::array::c_style> MemoryLabelGenerator::generate_labels(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
  return _gen_label(
      batch, [this](ssize_t num_triples, const int64_t* batch, float* data) {
        for (size_t i = 0; i < num_triples; i++) {
          auto base_adr = batch + i * detail::kNumTripleElements;
          auto h = *base_adr;
          auto r = *(base_adr + detail::kTripleRelationOffestInABatch);
          auto t = *(base_adr + detail::kTripleTailOffestInABatch);
          data[i] = (triples_.find(TripleIndex(h, r, t)) == triples_.end()
                         ? false_label_value_
                         : true_label_value_);
        }
      });
}

py::array_t<float, py::array::c_style> StaticLabelGenerator::generate_labels(
    py::array_t<int64_t, py::array::c_style | py::array::forcecast>& batch)
{
  return _gen_label(
      batch, [this](ssize_t num_triples, const int64_t* batch, float* data) {
        for (size_t i = 0; i < num_triples; i++) {
          data[i] = true_label_ ? true_label_value_ : false_label_value_;
        }
      });
}

py::array_t<float, py::array::c_style> StaticLabelGenerator::generate_labels(
    py::tuple shape)
{
  vector<ssize_t> shape_vec;
  int64_t num_triples = 1;
  for (auto const& v : shape) {
    auto val = v.cast<ssize_t>();
    shape_vec.push_back(val);
    num_triples *= val;
  }
  return _gen_label_from_shape(
      shape_vec, num_triples, nullptr,
      [this](ssize_t num_triples, const int64_t* batch, float* data) {
        for (size_t i = 0; i < num_triples; i++) {
          data[i] = true_label_ ? true_label_value_ : false_label_value_;
        }
      });
}
}  // namespace kgedata
