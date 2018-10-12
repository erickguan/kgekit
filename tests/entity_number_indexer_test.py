import unittest
import kgekit.io

class EntityNumberIndexerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        t, _ = kgekit.io.read_triples('tests/fixtures/triple.txt', "hrt", ' ')
        cls._indexer = kgekit.EntityNumberIndexer(t, "hrt")

    def test_attrs(self):
        self.assertEqual(self._indexer.entityIdMap(), {'/m/entity1': 0, '/m/entity2': 1, '/m/entity3': 2})
        self.assertEqual(self._indexer.relationIdMap(), {'/country': 1, '/produced_by': 0, '/produced_in': 2})
        self.assertEqual(self._indexer.indexes(), [kgekit.TripleIndex([0, 0, 1]), kgekit.TripleIndex([1, 1, 2]), kgekit.TripleIndex([0, 2, 2])])
        self.assertEqual(self._indexer.entities(), ['/m/entity1', '/m/entity2', '/m/entity3'])
        self.assertEqual(self._indexer.relations(), ['/produced_by', '/country', '/produced_in'])

if __name__ == '__main__':
    unittest.main()
