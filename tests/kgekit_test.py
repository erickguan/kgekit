import unittest
import kgekit

class MainTest(unittest.TestCase):
    def test_get_triple_index(self):
        self.assertEqual(kgekit.get_triple_index("1 2 3", "hrt", ' '), (1, 2, 3))
        self.assertEqual(kgekit.get_triple_index("1 2 3", "htr", ' '), (1, 3, 2))


if __name__ == '__main__':
    unittest.main()
