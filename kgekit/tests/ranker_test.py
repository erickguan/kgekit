import unittest
import kgekit
import pickle
import os
import numpy as np
import pytest

@pytest.mark.numpyfile
class RankerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        prefix = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures')
        cls.train, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_train.txt'), "hrt", ' ')
        cls.valid, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_valid.txt'), "hrt", ' ')
        cls.test, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_test.txt'), "hrt", ' ')
        cls.ranker = kgekit.Ranker(cls.train, cls.valid, cls.test)

    def test_rank_head(self):
        ranks = self.ranker.rankHead(np.array([
            0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
        ], dtype=np.float32), kgekit.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (5, 4))

    def test_rank_tail(self):
        ranks = self.ranker.rankTail(np.array([
            0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
        ], dtype=np.float32), kgekit.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (5, 5))

    def test_rank_relation(self):
        ranks = self.ranker.rankRelation(np.array([
            0.001, 0.002, 0.003, 0.004
        ], dtype=np.float32), kgekit.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (3, 2))

if __name__ == '__main__':
    unittest.main()
