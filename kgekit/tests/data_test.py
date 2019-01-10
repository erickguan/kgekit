import unittest
import kgedata
import kgekit.data

class DataTest(unittest.TestCase):
    def test_unwrap(self):
        self.assertEqual(kgekit.data.unpack(kgedata.TripleIndex(1, 2, 3)), (1, 2, 3))
        self.assertEqual(kgekit.data.unpack(kgedata.Triple("/m/1", "/m/2", "/m/3")), ("/m/1", "/m/2", "/m/3"))

if __name__ == '__main__':
    unittest.main()
