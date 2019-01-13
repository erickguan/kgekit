import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest

@pytest.mark.numpyfile
class CWASamplerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.triple_indexes, _ = kgekit.io.read_triple_indexes_numpy(cls.triple_index_filename, triple_order="hrt", delimiter=' ')
        cls.sampler = kgedata.CWASampler(5, 2, True)
        cls.label_generators = kgedata.LabelGenerator(cls.triple_indexes)

    def test_labels(self):
        batch_size = 2
        triple_indexes = self.triple_indexes[:batch_size]
        samples = self.sampler.sample(np.array([False, True], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4], [0, 0, 3], [0, 1, 3]],
            [[0, 0, 1], [1, 0, 1], [2, 0, 1], [3, 0, 1], [4, 0, 1], [0, 0, 1], [0, 1, 1]],
        ], dtype=np.int64))
        labels = self.label_generators.generate_labels(samples)
        np.testing.assert_equal(labels, np.array([
            [-1, -1, -1, 1, 1, -1, 1],
            [1, -1, -1, 1, -1, 1, -1],
        ], dtype=np.int64))

    def test_entity_labels(self):
        batch_size = 2
        triple_indexes = self.triple_indexes[:batch_size]
        samples = kgedata.CWASampler(5, 2, False).sample(np.array([False, False], dtype=np.bool), triple_indexes)
        np.testing.assert_equal(samples, np.array([
            [[0, 1, 0], [0, 1, 1], [0, 1, 2], [0, 1, 3], [0, 1, 4]],
            [[0, 0, 0], [0, 0, 1], [0, 0, 2], [0, 0, 3], [0, 0, 4]],
        ], dtype=np.int64))
        labels = self.label_generators.generate_labels(samples)
        np.testing.assert_equal(labels, np.array([
            [-1, -1, -1, 1, 1],
            [-1, 1, -1, -1, -1],
        ], dtype=np.int64))

    def test_shapes(self):
        samples = np.array([[0, 1, 2], [0, 1, 3]], dtype=np.int64) # 2, 3
        res = self.label_generators.generate_labels(samples)
        self.assertEqual(res.shape, (2,))
        np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))

        res = self.label_generators.generate_labels(samples.reshape(1, 2, -1))
        self.assertEqual(res.shape, (1,2))
        np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))

        res = self.label_generators.generate_labels(samples.reshape(2, 1, -1))
        self.assertEqual(res.shape, (2,1))
        np.testing.assert_equal(res.flatten(), np.array([-1, 1], dtype=np.int64))


if __name__ == '__main__':
    unittest.main()
