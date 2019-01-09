import unittest
import kgedata
import pickle

class KgekitTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.index = (1, 2, 3)
        cls.triple_index = kgedata.TripleIndex()
        cls.triple_index.head = 1
        cls.triple_index.relation = 2
        cls.triple_index.tail = 3
        cls.literals = ("/m/1", "/m/2", "/m/3")
        cls.triple = kgedata.Triple()
        cls.triple.head = "/m/1"
        cls.triple.relation = "/m/2"
        cls.triple.tail = "/m/3"

    def test_triple_index(self):
        self.assertEqual(kgedata.TripleIndex(*self.index), self.triple_index)

    def test_triple(self):
        self.assertEqual(kgedata.Triple(*self.literals), self.triple)

    def test_get_triple_index(self):
        self.assertEqual(kgedata.get_triple_index("1 2 3", "hrt", ' '), kgedata.TripleIndex(1, 2, 3))
        self.assertEqual(kgedata.get_triple_index("1 2 3", "htr", ' '), kgedata.TripleIndex(1, 3, 2))

    def test_get_triple(self):
        self.assertEqual(kgedata.get_triple("/m/1 /m/2 /m/3", "hrt", ' '), kgedata.Triple("/m/1", "/m/2", "/m/3"))
        self.assertEqual(kgedata.get_triple("/m/1 /m/2 /m/3", "htr", ' '), kgedata.Triple("/m/1", "/m/3", "/m/2"))

    def test_pickles(self):
        self.assertEqual(pickle.loads(pickle.dumps(self.triple_index, 2)), self.triple_index)
        self.assertEqual(pickle.loads(pickle.dumps(self.triple, 2)), self.triple)


if __name__ == '__main__':
    unittest.main()
