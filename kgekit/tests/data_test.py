import unittest
import kgedata
import kgekit.data
import pytest
import numpy as np

@pytest.mark.numpyfile
class DataTest(unittest.TestCase):
    def test_unwrap(self):
        self.assertEqual(kgekit.data.unpack(kgedata.TripleIndex(1, 2, 3)), (1, 2, 3))
        self.assertEqual(kgekit.data.unpack(kgedata.Triple("/m/1", "/m/2", "/m/3")), ("/m/1", "/m/2", "/m/3"))

    def test_transform_triple_numpy(self):
        np.testing.assert_equal(
            kgekit.data._transform_triple_numpy(kgedata.TripleIndex(1, 2, 3)),
            np.array([1,2,3], dtype=np.int64))

    def test_pack_triples_numpy(self):
        np.testing.assert_equal(
            kgekit.data.pack_triples_numpy([kgedata.TripleIndex(1, 2, 3), kgedata.TripleIndex(4, 5, 6)]),
            np.array([[1,2,3], [4,5,6]], dtype=np.int64))

if __name__ == '__main__':
    unittest.main()
