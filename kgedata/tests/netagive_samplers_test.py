import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest

@pytest.mark.numpyfile
class LCWANoThrowSamplerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.triple_indexes, _ = kgekit.io.read_triple_indexes_numpy(cls.triple_index_filename, triple_order="hrt", delimiter=' ')
        cls.entity_sampler = kgedata.LCWANoThrowSampler(cls.triple_indexes, 5, 2, 2, 0, 100, kgedata.LCWANoThrowSamplerStrategy.Hash)
        cls.relation_sampler = kgedata.LCWANoThrowSampler(cls.triple_indexes, 5, 2, 0, 1, 100, kgedata.LCWANoThrowSamplerStrategy.Hash)
        cls.combined_sampler = kgedata.LCWANoThrowSampler(cls.triple_indexes, 5, 2, 1, 1, 100, kgedata.LCWANoThrowSamplerStrategy.Hash)

    def test_num_negative_samples(self):
        self.assertEqual(self.entity_sampler.numNegativeSamples(), 2)
        self.assertEqual(self.relation_sampler.numNegativeSamples(), 1)
        self.assertEqual(self.combined_sampler.numNegativeSamples(), 2)

    def test_entity_sample(self):
        batch_size = 2
        triple_indexes = self.triple_indexes[:batch_size]
        samples = self.entity_sampler.sample(np.array([True, False], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [4, 1, 3]],
            [[2, 0, 1], [2, 0, 1]]
        ], dtype=np.int64))

    def test_relation_sample(self):
        batch_size = 1
        triple_indexes = self.triple_indexes[:batch_size]
        samples = self.relation_sampler.sample(np.array([True], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 0, 3]]
        ], dtype=np.int64))

    def test_combined_sample(self):
        batch_size = 2
        samples_per_batch = self.combined_sampler.numNegativeSamples()
        triple_indexes = self.triple_indexes[:batch_size]
        samples = self.combined_sampler.sample(np.array([True, False], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [0, 0, 3]],
            [[2, 0, 1], [0, 1, 1]]
        ], dtype=np.int64))


@pytest.mark.numpyfile
class CWASamplerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.triple_indexes, _ = kgekit.io.read_triple_indexes_numpy(cls.triple_index_filename, triple_order="hrt", delimiter=' ')
        cls.sampler = kgedata.CWASampler(cls.triple_indexes, 5, 2, True)
        cls.entity_sampler = kgedata.CWASampler(cls.triple_indexes, 5, 2, False)

    def test_sample(self):
        batch_size = 2
        triple_indexes = self.triple_indexes[:batch_size]
        samples, labels = self.sampler.sample(np.array([False, True], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4], [0, 0, 3], [0, 1, 3]],
            [[0, 0, 1], [1, 0, 1], [2, 0, 1], [3, 0, 1], [4, 0, 1], [0, 0, 1], [0, 1, 1]],
        ], dtype=np.int64))
        np.testing.assert_equal(labels, np.array([
            [False, False, False, True, True, False, True],
            [True, False, False, True, False, True, False],
        ], dtype=np.bool))

    def test_entity_sample(self):
        batch_size = 2
        triple_indexes = self.triple_indexes[:batch_size]
        samples, labels = self.entity_sampler.sample(np.array([False, False], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4]],
            [[0, 0, 0], [0, 0, 1], [0, 0, 2], [0, 0, 3], [0, 0, 4]],
        ], dtype=np.int64))
        np.testing.assert_equal(labels, np.array([
            [False, False, False, True, True],
            [False, True, False, False, False],
        ], dtype=np.bool))

if __name__ == '__main__':
    unittest.main()
