import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest


@pytest.mark.numpyfile
@pytest.fixture(scope="module")
def triple_indexes():
  triple_index_filename = os.path.join(
      os.path.dirname(os.path.realpath(__file__)), 'fixtures',
      'corruptor_triple.txt')
  a, _ = kgekit.io.read_triple_indexes_numpy(
      triple_index_filename, triple_order="hrt", delimiter=' ')
  return a


@pytest.fixture(scope="module")
def sampler():
  return kgedata.CWASampler(5, 2, True)


@pytest.fixture(scope="module")
def memory_label_generators(triple_indexes):
  return kgedata.MemoryLabelGenerator(triple_indexes)


@pytest.fixture(scope="module")
def static_false_label_generator():
  return kgedata.StaticLabelGenerator(False)


@pytest.fixture(scope="module")
def static_true_label_generator():
  return kgedata.StaticLabelGenerator(True)

def test_labels(sampler, triple_indexes, memory_label_generators):
  batch_size = 2
  triple_indexes = triple_indexes[:batch_size]
  samples = sampler(np.array([False, True], dtype=np.bool), triple_indexes)
  np.testing.assert_equal(
      samples,
      np.array([
          [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4], [0, 0, 3],
           [0, 1, 3]],
          [[0, 0, 1], [1, 0, 1], [2, 0, 1], [3, 0, 1], [4, 0, 1], [0, 0, 1],
           [0, 1, 1]],
      ],
               dtype=np.int64))
  labels = memory_label_generators(samples)
  np.testing.assert_equal(
      labels,
      np.array([
          [-1, -1, -1, 1, 1, -1, 1],
          [1, -1, -1, 1, -1, 1, -1],
      ],
               dtype=np.float32))


def test_entity_labels(triple_indexes, memory_label_generators):
  batch_size = 2
  sampled_triple_indexes = triple_indexes[:batch_size]
  samples = kgedata.CWASampler(5, 2, False).sample(
      np.array([False, False], dtype=np.bool), sampled_triple_indexes)
  np.testing.assert_equal(
      samples,
      np.array([
          [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4]],
          [[0, 0, 0], [0, 0, 1], [0, 0, 2], [0, 0, 3], [0, 0, 4]],
      ],
               dtype=np.int64))
  labels = memory_label_generators(samples)
  np.testing.assert_equal(
      labels,
      np.array([
          [-1, -1, -1, 1, 1],
          [-1, 1, -1, -1, -1],
      ], dtype=np.float32))


def test_shapes(memory_label_generators):
  samples = np.array([[0, 1, 2], [0, 1, 3]], dtype=np.int64)  # 2, 3
  res = memory_label_generators(samples)
  assert res.shape == (2,)
  np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))

  res = memory_label_generators(samples.reshape(1, 2, -1))
  assert res.shape == (1, 2)
  np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))

  res = memory_label_generators(samples.reshape(2, 1, -1))
  assert res.shape == (2, 1)
  np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))


def test_static_label_generator(triple_indexes,
                                    static_false_label_generator, static_true_label_generator):
  batch_size = 2
  sampled_triple_indexes = triple_indexes[:batch_size]
  samples = kgedata.CWASampler(5, 2, False).sample(
      np.array([False, False], dtype=np.bool), sampled_triple_indexes)
  np.testing.assert_equal(
      samples,
      np.array([
          [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4]],
          [[0, 0, 0], [0, 0, 1], [0, 0, 2], [0, 0, 3], [0, 0, 4]],
      ],
               dtype=np.int64))
  labels = static_false_label_generator(samples)
  np.testing.assert_equal(
      labels,
      np.array([
          [-1, -1, -1, -1, -1],
          [-1, -1, -1, -1, -1],
      ], dtype=np.float32))
  labels = static_true_label_generator(samples)
  np.testing.assert_equal(
      labels,
      np.array([
          [1, 1, 1, 1, 1],
          [1, 1, 1, 1, 1],
      ], dtype=np.float32))
  labels = static_true_label_generator(samples.shape)
  np.testing.assert_equal(
      labels,
      np.array([
          [1, 1, 1, 1, 1],
          [1, 1, 1, 1, 1],
      ], dtype=np.float32))
