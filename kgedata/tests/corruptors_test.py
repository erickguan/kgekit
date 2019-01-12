import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest

pytestmark = pytest.mark.random_order(disabled=True)

@pytest.mark.numpyfile
class CorruptorsTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.tolerance_margin = 0.05
        cls.repeat = 100000
        cls.num_corrupts = 2
        cls.raw_indexes, _ = kgekit.io.read_triple_indexes_numpy(cls.triple_index_filename, triple_order="hrt", delimiter=' ')
        cls.triple_indexes = np.repeat(cls.raw_indexes, cls.repeat, axis=0)

    def test_bernoulli(self):
        num_rel = set(self.raw_indexes[:, 1])
        corruptor = kgedata.BernoulliCorruptor(self.raw_indexes, len(num_rel), self.num_corrupts)
        choices = corruptor.make_random_choice(self.triple_indexes)
        for i in range(len(self.raw_indexes)):
            prob = corruptor.get_probability_relation(self.raw_indexes[i,1])
            count = np.bincount(choices[i*self.repeat:(i+1)*self.repeat,:].flatten())
            self.assertTrue(-self.tolerance_margin < (count[0] / self.repeat/self.num_corrupts - prob) < self.tolerance_margin)

    def test_uniform(self):
        corruptor = kgedata.UniformCorruptor(self.num_corrupts)
        choices = corruptor.make_random_choice(self.triple_indexes)
        # the ratio should be close
        count = np.bincount(choices.flatten())
        self.assertTrue(-self.tolerance_margin < (count[0] / count[1] - 1) < self.tolerance_margin)



if __name__ == '__main__':
    unittest.main()
