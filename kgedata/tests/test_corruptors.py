import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest

pytestmark = pytest.mark.random_order(disabled=True)


@pytest.fixture(scope="module")
def num_corrupts():
  return 2


@pytest.fixture(scope="module")
def repeat():
  return 100000


@pytest.fixture(scope="module")
def tolerance_margin():
  return 0.05


@pytest.fixture(scope="module")
def raw_indexes():
  triple_index_filename = os.path.join(
      os.path.dirname(os.path.realpath(__file__)), 'fixtures',
      'corruptor_triple.txt')
  a, _ = kgekit.io.read_triple_indexes_numpy(
      triple_index_filename, triple_order="hrt", delimiter=' ')
  return a


@pytest.fixture(scope="module")
def triple_indexes(raw_indexes, repeat):
  return np.repeat(raw_indexes, repeat, axis=0)


def test_bernoulli(raw_indexes, num_corrupts, triple_indexes, tolerance_margin,
                   repeat):
  num_rel = set(raw_indexes[:, 1])
  corruptor = kgedata.BernoulliCorruptor(raw_indexes, len(num_rel),
                                         num_corrupts)
  choices = corruptor.make_random_choice(triple_indexes)
  for i in range(len(raw_indexes)):
    prob = corruptor.get_probability_relation(raw_indexes[i, 1])
    count = np.bincount(choices[i * repeat:(i + 1) * repeat, :].flatten())
    assert (-tolerance_margin <
            (count[0] / repeat / num_corrupts - prob) < tolerance_margin)


def test_uniform(num_corrupts, triple_indexes, tolerance_margin):
  corruptor = kgedata.UniformCorruptor(num_corrupts)
  choices = corruptor.make_random_choice(triple_indexes)
  # the ratio should be close
  count = np.bincount(choices.flatten())
  assert (-tolerance_margin < (count[0] / count[1] - 1) < tolerance_margin)
