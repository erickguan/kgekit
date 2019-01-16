import unittest
import kgekit.io
import kgedata
import pickle
import os.path
import numpy as np
import pytest

# I used this to test ranker pickle
# import os
# import pickle
# import kgekit.io

# prefix = os.path.join('kgedata', 'tests', 'fixtures')
# train, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_train.txt'), "hrt", ' ')
# valid, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_valid.txt'), "hrt", ' ')
# test, _ = kgekit.io.read_triple_indexes(os.path.join(prefix, 'ranker_test.txt'), "hrt", ' ')
# ranker = kgedata.Ranker(train, valid, test)
# ranker.export_state()

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

@pytest.fixture(scope="module")
def ranker():
    prefix = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures')
    train, _ = kgekit.io.read_triple_indexes_numpy(os.path.join(prefix, 'ranker_train.txt'), triple_order="hrt", delimiter=' ')
    valid, _ = kgekit.io.read_triple_indexes_numpy(os.path.join(prefix, 'ranker_valid.txt'), triple_order="hrt", delimiter=' ')
    test, _ = kgekit.io.read_triple_indexes_numpy(os.path.join(prefix, 'ranker_test.txt'), triple_order="hrt", delimiter=' ')
    return kgedata.Ranker(train, valid, test)

def test_rank_head(ranker):
    ranks = ranker.rank_head(np.array([
        0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
    ], dtype=np.float32), np.array([4, 2, 3], dtype=np.int64))
    assert ranks == (5, 4)

    ranks = ranker.rank_head(np.array([
        0.998, 0.032, 0.003, 0.184, 0.265, 0.356, 0.237
    ], dtype=np.float32), np.array([5, 2, 3], dtype=np.int64))
    assert ranks == (6, 4)

def test_rank_tail(ranker):
    ranks = ranker.rank_tail(np.array([
        0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007
    ], dtype=np.float32), np.array([4, 2, 3], dtype=np.int64))
    assert ranks == (4, 4)

    ranks = ranker.rank_tail(np.array([
        0.998, 0.032, 0.003, 0.184, 0.265, 0.356, 0.237
    ], dtype=np.float32), np.array([1, 2, 4], dtype=np.int64))
    assert ranks == (5, 4)

def test_rank_relation(ranker):
    ranks = ranker.rank_relation(np.array([
        0.001, 0.002, 0.003, 0.004
    ], dtype=np.float32), np.array([4, 2, 3], dtype=np.int64))
    assert ranks == (3, 2)

    ranks = ranker.rank_relation(np.array([
        0.231, 0.562, 0.923, 0.344
    ], dtype=np.float32), np.array([3, 1, 5], dtype=np.int64))
    assert ranks == (3, 2)

# This method has to be correct to ensure multiprocessing correctness
def test_pickle(ranker):
    ranker_bytes = pickle.dumps(ranker)
    state = ranker.export_state()
    loaded_ranker = pickle.loads(ranker_bytes)

    test_rank_head(loaded_ranker)
    test_rank_relation(loaded_ranker)
    test_rank_tail(loaded_ranker)

    assert _compare_state(state, ranker.export_state())
