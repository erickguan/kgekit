import unittest
import kgekit
import pickle
import os
import numpy as np
import pytest

@pytest.mark.numpyfile
class LCWANoThrowSamplerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.triple_indexes, _ = kgekit.io.read_triple_indexes(cls.triple_index_filename, "hrt", ' ')
        cls.entity_sampler = kgekit.LCWANoThrowSampler(cls.triple_indexes, 2, 0, kgekit.LCWANoThrowSamplerStrategy.Hash)
        cls.relation_sampler = kgekit.LCWANoThrowSampler(cls.triple_indexes, 0, 1, kgekit.LCWANoThrowSamplerStrategy.Hash)
        cls.combined_sampler = kgekit.LCWANoThrowSampler(cls.triple_indexes, 1, 1, kgekit.LCWANoThrowSamplerStrategy.Hash)

    def test_num_negative_samples(self):
        self.assertEqual(self.entity_sampler.numNegativeSamples(), 2)
        self.assertEqual(self.relation_sampler.numNegativeSamples(), 1)
        self.assertEqual(self.combined_sampler.numNegativeSamples(), 2)

    def test_entity_sample(self):
        batch_size = 2
        samples_per_batch = 1 + self.entity_sampler.numNegativeSamples()
        samples = np.zeros((batch_size, samples_per_batch, 3), dtype=np.int32)
        triple_indexes = self.triple_indexes[:batch_size]
        self.entity_sampler.sample(samples, [True, False], triple_indexes, 1000)
        np.testing.assert_array_equal(samples, np.array([
            [
                [0, 1, 3], [0, 1, 2], [0, 1, 1]
            ], [
                [0, 0, 1], [1, 0, 1], [4, 0, 1]
            ]
        ], dtype=np.int32))

    def test_relation_sample(self):
        batch_size = 1
        samples_per_batch = 1 + self.relation_sampler.numNegativeSamples()
        samples = np.zeros((batch_size, samples_per_batch, 3), dtype=np.int32)
        triple_indexes = self.triple_indexes[:batch_size]
        self.relation_sampler.sample(samples, [True], triple_indexes, 100)
        np.testing.assert_array_equal(samples, np.array([
            [
                [0, 1, 3], [0, 0, 3]
            ]
        ], dtype=np.int32))

    def test_combined_sample(self):
        batch_size = 2
        samples_per_batch = 1 + self.combined_sampler.numNegativeSamples()
        samples = np.zeros((batch_size, samples_per_batch, 3), dtype=np.int32)
        triple_indexes = self.triple_indexes[:batch_size]
        self.combined_sampler.sample(samples, [True, False], triple_indexes, 100)
        np.testing.assert_array_equal(samples, np.array([
            [
                [0, 1, 3], [0, 1, 0], [0, 0, 3]
            ],
            [
                [0, 0, 1], [2, 0, 1], [0, 1, 1]
            ]
        ], dtype=np.int32))


if __name__ == '__main__':
    unittest.main()
