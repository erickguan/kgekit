import unittest
import kgekit

class KgekitTest(unittest.TestCase):
    def test_get_triple_index(self):
        self.assertEqual(kgekit.get_triple_index("1 2 3", "hrt", ' '), kgekit.TripleIndex(1, 2, 3))
        self.assertEqual(kgekit.get_triple_index("1 2 3", "htr", ' '), kgekit.TripleIndex(1, 3, 2))

    def test_get_triple(self):
        self.assertEqual(kgekit.get_triple("/m/1 /m/2 /m/3", "hrt", ' '), kgekit.Triple("/m/1", "/m/2", "/m/3"))
        self.assertEqual(kgekit.get_triple("/m/1 /m/2 /m/3", "htr", ' '), kgekit.Triple("/m/1", "/m/3", "/m/2"))

if __name__ == '__main__':
    unittest.main()
