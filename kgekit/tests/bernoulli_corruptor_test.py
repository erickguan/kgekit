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
        cls.corruptor = kgekit.BernoulliCorruptor(cls.triple_indexes)

    def test_getProbablityRelation(self):
        with pytest.raises(IndexError):
            self.corruptor.getProbablityRelation(-1)

        with pytest.raises(IndexError):
            self.corruptor.getProbablityRelation(99999999)

        first_rel = self.corruptor.getProbablityRelation(0)
        second_rel = self.corruptor.getProbablityRelation(1)
        self.assertAlmostEqual(first_rel[0], 1/2.5)
        self.assertAlmostEqual(first_rel[1], 1.5/2.5)

        self.assertAlmostEqual(second_rel[0], 2/3.0)
        self.assertAlmostEqual(second_rel[1], 1/3.0)

if __name__ == '__main__':
    unittest.main()
