import unittest
import kgedata
import numpy as np
import pytest

@pytest.mark.numpyfile
class TripleExpanderTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.batch = np.array([[1,0,2], [0,1,2]], dtype=np.int64)
        cls.num_entity = 3
        cls.num_relation = 2

    def test_expand_triple_batch(self):
        expand1, splits1 = kgedata.expand_triple_batch(self.batch, self.num_entity, self.num_relation, True, True)
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
        self.assertEqual(splits1, [(0, 3, 6, 8), (8, 11, 14, 16)])

        expand2, splits2 = kgedata.expand_triple_batch(self.batch, self.num_entity, self.num_relation, False, True)
        np.testing.assert_array_equal(expand2, np.array([
            [1, 0, 2], # 0
            [1, 1, 2], # 1
            [0, 0, 2], # 2
            [0, 1, 2],
        ], dtype=np.int64))
        self.assertEqual(splits2, [(0, 0, 0, 2), (2, 2, 2, 4)])

        expand3, splits3 = kgedata.expand_triple_batch(self.batch, self.num_entity, self.num_relation, True, False)
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
        self.assertEqual(splits3, [(0, 3, 6, 6), (6, 9, 12, 12)])

        expand4, splits4 = kgedata.expand_triple_batch(self.batch, self.num_entity, self.num_relation, False, False)
        np.testing.assert_array_equal(expand4, np.array([], dtype=np.int64).reshape((0,3)))
        self.assertEqual(splits4, [(0, 0, 0, 0), (0, 0, 0, 0)])
