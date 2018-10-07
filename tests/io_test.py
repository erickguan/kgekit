import unittest
import os
from kgekit import io

class MainTest(unittest.TestCase):
    triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple_index.txt')
    triple_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple.txt')

    def test_read_triple_indexes(self):
        self.assertEqual(io.read_triple_indexes(self.triple_index_filename, "hrt", ' '), [(1, 2, 3), (9, 1, 3)])
        self.assertEqual(io.read_triple_indexes(self.triple_index_filename, "htr", ' '), [(1, 3, 2), (9, 3, 1)]) # different order
        self.assertEqual(io.read_triple_indexes(self.triple_index_filename, "hrt", '\t'), []) # can't parse
        with self.assertRaises(FileNotFoundError):
            io.read_triple_indexes("", "hrt", ' ') # wrong file
        with self.assertRaises(RuntimeError):
            io.read_triple_indexes(self.triple_index_filename, "hrr", ' ') # wrong order

    def test_read_triple(self):
        self.assertEqual(io.read_triples(self.triple_filename, "hrt", ' '), [("/m/entity1", "/produced_by", "/m/entity2"), ("/m/entity2", "/country", "/m/entity3"), ("/m/entity1", "/produced_in", "/m/entity3")])
        self.assertEqual(io.read_triples(self.triple_filename, "htr", ' '), [("/m/entity1", "/m/entity2", "/produced_by"), ("/m/entity2", "/m/entity3", "/country"), ("/m/entity1", "/m/entity3", "/produced_in")]) # different order
        self.assertEqual(io.read_triples(self.triple_filename, "hrt", '\t'), []) # can't parse
        with self.assertRaises(FileNotFoundError):
            io.read_triples("", "hrt", ' ') # wrong file
        with self.assertRaises(RuntimeError):
            io.read_triples(self.triple_filename, "hrr", ' ') # wrong order


if __name__ == '__main__':
    unittest.main()
