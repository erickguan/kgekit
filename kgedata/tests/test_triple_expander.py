import unittest
import kgedata
import numpy as np
import pytest

@pytest.mark.numpyfile

@pytest.fixture(scope="module")
def batch_parameters():
    """batch, num_entity, num_relation"""
    return np.array([[1,0,2], [0,1,2]], dtype=np.int64), 3, 2

def test_expand_triple_batch(batch_parameters):
    batch, num_entity, num_relation = batch_parameters
    expand1, splits1 = kgedata.expand_triple_batch(batch, num_entity, num_relation)
    np.testing.assert_array_equal(expand1, np.array([
        [0, 0, 2], #0
        [1, 0, 2],
        [2, 0, 2],
        [1, 0, 0], #3
        [1, 0, 1],
        [1, 0, 2],
        [1, 0, 2], # 6
        [1, 1, 2], # 7
        [0, 1, 2], # 8
        [1, 1, 2],
        [2, 1, 2],
        [0, 1, 0], # 11
        [0, 1, 1],
        [0, 1, 2],
        [0, 0, 2], # 14
        [0, 1, 2],
    ], dtype=np.int64))
    np.testing.assert_array_equal(splits1, np.array([
        [0, 3, 6, 8],
        [8, 11, 14, 16]
    ], dtype=np.int64))

    expand2, splits2 = kgedata.expand_triple_batch(batch, num_entity, num_relation, False, True)
    np.testing.assert_array_equal(expand2, np.array([
        [1, 0, 2], # 0
        [1, 1, 2], # 1
        [0, 0, 2], # 2
        [0, 1, 2],
    ], dtype=np.int64))
    np.testing.assert_array_equal(splits2, np.array([
        [0, 0, 0, 2],
        [2, 2, 2, 4]
    ], dtype=np.int64))

    expand3, splits3 = kgedata.expand_triple_batch(batch, num_entity, num_relation, True, False)
    np.testing.assert_array_equal(expand3, np.array([
        [0, 0, 2], #0
        [1, 0, 2],
        [2, 0, 2],
        [1, 0, 0], #3
        [1, 0, 1],
        [1, 0, 2],
        [0, 1, 2], # 6
        [1, 1, 2],
        [2, 1, 2],
        [0, 1, 0], # 9
        [0, 1, 1],
        [0, 1, 2],
    ], dtype=np.int64))
    np.testing.assert_array_equal(splits3, np.array([
        [0, 3, 6, 6],
        [6, 9, 12, 12]
    ]), dtype=np.int64)

    expand4, splits4 = kgedata.expand_triple_batch(batch, num_entity, num_relation, False, False)
    np.testing.assert_array_equal(expand4, np.array([], dtype=np.int64).reshape((0,3)))
    np.testing.assert_array_equal(splits4, np.array([
        [0, 0, 0, 0],
        [0, 0, 0, 0]
    ]), dtype=np.int64)
