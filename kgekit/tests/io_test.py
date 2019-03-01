import unittest
import os
import kgekit.io
import kgedata
import pytest
import numpy as np

class MainTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.triple_index_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple_index.txt')
        cls.triple_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'triple.txt')
        cls.label_filename = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', 'labels.txt')
        t, _ = kgekit.io.read_triples(cls.triple_filename, triple_order="hrt", delimiter=' ')
        cls.triples = [kgedata.Triple("/m/entity1", "/produced_by", "/m/entity2"),
                        kgedata.Triple("/m/entity2", "/country", "/m/entity3"),
                        kgedata.Triple("/m/entity1", "/produced_in", "/m/entity3"),
                        kgedata.Triple("/m/entity3", "/produced_in", "/m/entity4"),
                        kgedata.Triple("/m/entity1", "/country", "/m/entity4")]
        cls.indexer = kgedata.EntityNumberIndexer(t, "hrt")

    @pytest.fixture(autouse=True)
    def initdir(self, tmpdir):
        tmpdir.chdir()

    def test_read_triple_indexes(self):
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, triple_order="hrt", delimiter=' '),
                ([kgedata.TripleIndex(1, 2, 3), kgedata.TripleIndex(9, 1, 3)], 0))
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, triple_order="htr", delimiter=' '),
                ([kgedata.TripleIndex(1, 3, 2), kgedata.TripleIndex(9, 3, 1)], 0)) # different order
        self.assertEqual(kgekit.io.read_triple_indexes(self.triple_index_filename, triple_order="hrt", delimiter='\t'),
                ([], 2)) # can't parse
        with self.assertRaises(FileNotFoundError):
            kgekit.io.read_triple_indexes("", triple_order="hrt", delimiter=' ') # wrong file
        with self.assertRaises(RuntimeError):
            kgekit.io.read_triple_indexes(self.triple_index_filename, triple_order="hrr", delimiter=' ') # wrong order

    def test_read_triple_indexes_numpy(self):
        np.testing.assert_equal(kgekit.io.read_triple_indexes_numpy(self.triple_index_filename, triple_order="hrt", delimiter=' ')[0],
                np.array([[1, 2, 3], [9, 1, 3]], dtype=np.int64))
        np.testing.assert_equal(kgekit.io.read_triple_indexes_numpy(self.triple_index_filename, triple_order="htr", delimiter=' ')[0],
                np.array([[1, 3, 2], [9, 3, 1]], dtype=np.int64))
        arr, num_failed = kgekit.io.read_triple_indexes_numpy(self.triple_index_filename, triple_order="hrt", delimiter='\t')
        np.testing.assert_equal(arr, np.array([], dtype=np.int64)) # can't parse
        self.assertEqual(num_failed, 2)
        with self.assertRaises(FileNotFoundError):
            kgekit.io.read_triple_indexes_numpy("", triple_order="hrt", delimiter=' ') # wrong file
        with self.assertRaises(RuntimeError):
            kgekit.io.read_triple_indexes_numpy(self.triple_index_filename, triple_order="hrr", delimiter=' ') # wrong order

    def test_read_triple(self):
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, triple_order="hrt", delimiter=' '),
                ([kgedata.Triple("/m/entity1", "/produced_by", "/m/entity2"),
                    kgedata.Triple("/m/entity2", "/country", "/m/entity3"),
                    kgedata.Triple("/m/entity1", "/produced_in", "/m/entity3"),
                    kgedata.Triple("/m/entity3", "/produced_in", "/m/entity4"),
                    kgedata.Triple("/m/entity1", "/country", "/m/entity4")], 0))
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, triple_order="htr", delimiter=' '),
                ([kgedata.Triple("/m/entity1", "/m/entity2", "/produced_by"),
                    kgedata.Triple("/m/entity2", "/m/entity3", "/country"),
                    kgedata.Triple("/m/entity1", "/m/entity3", "/produced_in"),
                    kgedata.Triple("/m/entity3", "/m/entity4", "/produced_in"),
                    kgedata.Triple("/m/entity1", "/m/entity4", "/country")], 0)) # different order
        self.assertEqual(kgekit.io.read_triples(self.triple_filename, triple_order="hrt", delimiter='\t'),
                ([], 5)) # can't parse
        with self.assertRaises(FileNotFoundError):
            kgekit.io.read_triples("", triple_order="hrt", delimiter=' ') # wrong file
        with self.assertRaises(RuntimeError):
            kgekit.io.read_triples(self.triple_filename, triple_order="hrr", delimiter=' ') # wrong order

    def test_read_labels(self):
        self.assertEqual(kgekit.io.read_labels(self.label_filename, ' '), [("aa", '"eng"@eng'), ("bb", '"fr"@fr'), ('Gyulai_FC', '"Gyulai FC"@eng')])

    def test_write_index_translation(self):
        translation_filepath = "translation.txt"
        entities_ids = self.indexer.entityIdMap()
        relations_ids = self.indexer.relationIdMap()

        kgekit.io.write_index_translation(translation_filepath, entities_ids, relations_ids)
        entities_ids_read, relations_ids_read = kgekit.io.read_translation(translation_filepath)
        entities_ids_list = [(k,v) for k,v in entities_ids.items()]
        relations_ids_list = [(k,v) for k,v in relations_ids.items()]

        self.assertEqual(entities_ids_read, entities_ids_list)
        self.assertEqual(relations_ids_read, relations_ids_list)

    def test_write_triples(self):
        triple_filepath = "triple_out.txt"
        kgekit.io.write_triples(triple_filepath, self.triples)
        with open(triple_filepath) as f:
            print(f.read())
        triples, _ = kgekit.io.read_triples(triple_filepath)
        self.assertEqual(self.triples, triples)

        indexes = self.indexer.indexes()
        kgekit.io.write_triples(triple_filepath, indexes)
        triples, _ = kgekit.io.read_triple_indexes(triple_filepath)
        self.assertEqual(indexes, triples)


if __name__ == '__main__':
    unittest.main()
