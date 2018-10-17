import unittest
import os
import kgekit.io

class MainTest(unittest.TestCase):
    triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple_index.txt')
    triple_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple.txt')

    def test_read_triple_indexes(self):
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, "hrt", ' '),
                ([kgekit.TripleIndex(1, 2, 3), kgekit.TripleIndex(9, 1, 3)], 0))
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, "htr", ' '),
                ([kgekit.TripleIndex(1, 3, 2), kgekit.TripleIndex(9, 3, 1)], 0)) # different order
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, "hrt", '\t'),
                ([], 2)) # can't parse
        with self.assertRaises(FileNotFoundError):
            kgekit.io.read_triple_indexes("", "hrt", ' ') # wrong file
        with self.assertRaises(RuntimeError):
            kgekit.io.read_triple_indexes(self.triple_index_filename, "hrr", ' ') # wrong order

    def test_read_triple(self):
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, "hrt", ' '),
                ([kgekit.Triple("/m/entity1", "/produced_by", "/m/entity2"),
                    kgekit.Triple("/m/entity2", "/country", "/m/entity3"),
                    kgekit.Triple("/m/entity1", "/produced_in", "/m/entity3")], 0))
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, "htr", ' '),
                ([kgekit.Triple("/m/entity1", "/m/entity2", "/produced_by"),
                    kgekit.Triple("/m/entity2", "/m/entity3", "/country"),
                    kgekit.Triple("/m/entity1", "/m/entity3", "/produced_in")], 0)) # different order
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, "hrt", '\t'),
                ([], 3)) # can't parse
        with self.assertRaises(FileNotFoundError):
            kgekit.io.read_triples("", "hrt", ' ') # wrong file
        with self.assertRaises(RuntimeError):
            kgekit.io.read_triples(self.triple_filename, "hrr", ' ') # wrong order


if __name__ == '__main__':
    unittest.main()
