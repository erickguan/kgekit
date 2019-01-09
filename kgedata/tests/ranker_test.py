import unittest
import kgedata
import pickle
import os.path
import numpy as np
import pytest

# I used this to test ranker pickle
# import os
# import pickle
# import kgedata.io

# prefix = os.path.join('kgedata', 'tests', 'fixtures')
# train, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_train.txt'), "hrt", ' ')
# valid, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_valid.txt'), "hrt", ' ')
# test, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_test.txt'), "hrt", ' ')
# ranker = kgedata.Ranker(train, valid, test)
# ranker.exportState()

def _compose_pair_state(rest_keys, entry_list):
    s1 = frozenset(rest_keys)
    s2 = frozenset([frozenset(l) for l in entry_list])
    return frozenset([s1, s2])

def _calc_state(state):
    rest_heads, rest_tails, rest_relations = state
    return frozenset([
        _compose_pair_state(*rest_heads),
        _compose_pair_state(*rest_tails),
        _compose_pair_state(*rest_relations)
    ])

def _compare_state(orig_state, new_state):
    s1 = _calc_state(orig_state)
    s2 = _calc_state(new_state)
    return s1.issubset(s2) and s2.issubset(s1)

@pytest.mark.numpyfile
class RankerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        prefix = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures')
        cls.train, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_train.txt'), "hrt", ' ')
        cls.valid, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_valid.txt'), "hrt", ' ')
        cls.test, _ = kgedata.io.read_triple_indexes(os.path.join(prefix, 'ranker_test.txt'), "hrt", ' ')
        cls.ranker = kgedata.Ranker(cls.train, cls.valid, cls.test)

    def test_rank_head(self):
        ranks = self.ranker.rankHead(np.array([
            0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
        ], dtype=np.float32), kgedata.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (5, 4))

        ranks = self.ranker.rankHead(np.array([
            0.998, 0.032, 0.003, 0.184, 0.265, 0.356, 0.237
        ], dtype=np.float32), kgedata.TripleIndex(5, 2, 3))
        self.assertEqual(ranks, (6, 4))

    def test_rank_tail(self):
        ranks = self.ranker.rankTail(np.array([
            0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
        ], dtype=np.float32), kgedata.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (4, 4))

        ranks = self.ranker.rankTail(np.array([
            0.998, 0.032, 0.003, 0.184, 0.265, 0.356, 0.237
        ], dtype=np.float32), kgedata.TripleIndex(1, 2, 4))
        self.assertEqual(ranks, (5, 4))

    def test_rank_relation(self):
        ranks = self.ranker.rankRelation(np.array([
            0.001, 0.002, 0.003, 0.004
        ], dtype=np.float32), kgedata.TripleIndex(4, 2, 3))
        self.assertEqual(ranks, (3, 2))

        ranks = self.ranker.rankRelation(np.array([
            0.231, 0.562, 0.923, 0.344
        ], dtype=np.float32), kgedata.TripleIndex(3, 1, 5))
        self.assertEqual(ranks, (3, 2))

    # This method has to be correct to ensure multiprocessing correctness
    def test_pickle(self):
        ranker_bytes = pickle.dumps(self.ranker)
        state = self.ranker.exportState()
        self.ranker = pickle.loads(ranker_bytes)

        self.test_rank_head()
        self.test_rank_relation()
        self.test_rank_tail()

        self.assertTrue(_compare_state(state, self.ranker.exportState()))

if __name__ == '__main__':
    unittest.main()
