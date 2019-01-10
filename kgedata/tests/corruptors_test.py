import unittest
import kgedata
import kgekit.io
import pickle
import os
import numpy as np
import pytest

@pytest.mark.numpyfile
class CorruptorsTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'corruptor_triple.txt')
        cls.triple_indexes = []
        cls.tolerance_margin = 0.05
        cls.repeat = 100000
        cls.raw_indexes, _ = kgekit.io.read_triple_indexes(cls.triple_index_filename, triple_order="hrt", delimiter=' ')
        for x in cls.raw_indexes:
            for _ in range(cls.repeat):
                cls.triple_indexes.append(x)

    def test_bernoulli(self):
        num_rel = set()
        for i in self.raw_indexes:
            num_rel.add(i.relation)
        corruptor = kgedata.BernoulliCorruptor(self.raw_indexes, len(num_rel))
        choices = np.empty(len(self.triple_indexes), dtype=np.bool_)
        corruptor.make_random_choice(self.triple_indexes, choices)
        for i in range(len(self.raw_indexes)):
            prob = corruptor.get_probability_relation(self.raw_indexes[i].relation)
            count = np.bincount(choices[i*self.repeat:(i+1)*self.repeat])
            self.assertTrue(-self.tolerance_margin < (count[0] / self.repeat - prob) < self.tolerance_margin)

    def test_uniform(self):
        corruptor = kgedata.UniformCorruptor()
        choices = np.empty(len(self.triple_indexes), dtype=np.bool_)
        corruptor.make_random_choice(self.triple_indexes, choices)
        # the ratio should be close
        count = np.bincount(choices)
        self.assertTrue(-self.tolerance_margin < (count[0] / count[1] - 1) < self.tolerance_margin)



if __name__ == '__main__':
    unittest.main()
