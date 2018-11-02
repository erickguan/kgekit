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
        cls.sampler = kgekit.LCWANoThrowSampler(cls.triple_indexes, 2, 1)

    def test_num_negative_samples(self):
        self.assertEqual(self.sampler.numNegativeSamples(), 3)

    def test_sample(self):
        batch_size = 2
        samples_per_batch = 3
        samples = np.zeros((batch_size, samples_per_batch, 3), dtype=np.int32)
        self.sampler.sample(samples, [[True for _ in self.triple_indexes], self.triple_indexes], 100)
        np.testing.assert_array_equal(samples, np.array([
            [
                [0, 0, 1], [0, 0, 3], [0, 0, 3]
            ], [
                [0, 1, 3], [0, 1, 1], [0, 1, 2]
        ]], dtype=np.int32))


if __name__ == '__main__':
    unittest.main()
