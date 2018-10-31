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
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple_index.txt')
        cls.triple_indexes, _ = kgekit.io.read_triple_indexes(cls.triple_index_filename, "hrt", ' ')
        cls.sampler = kgekit.LCWANoThrowSampler(cls.triple_indexes, 2, 1, False)

    def test_num_negative_samples(self):
        self.assertEqual(self.sampler.numNegativeSamples(), 3)

    def test_sample(self):
        batch_size = 2
        samples_per_batch = 3
        samples = np.zeros((batch_size, 3, samples_per_batch), dtype=np.int32)
        print(self.sampler.sample(samples, self.triple_indexes))
        np.testing.assert_array_equal(samples, np.ones((batch_size, 3, samples_per_batch), dtype=np.int32))


if __name__ == '__main__':
    unittest.main()
