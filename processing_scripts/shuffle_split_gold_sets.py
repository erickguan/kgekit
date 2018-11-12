"""Shuffle and split the data into train/valid/test sets."""

import sys
import kgekit.data
import kgekit.io

def shuffled_split(filename):
    triples, num_failed = kgekit.io.read_triples(filename, 'hrt', ' ')
    if num_failed > 0:
        print("There is " + str(num_failed) + " failures.")
    indexer = kgekit.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    kgekit.io.write_index_translation('translation.protobuf', indexer.entityIdMap(), indexer.relationIdMap())
    indexes = kgekit.data.shuffle_triples(indexes)
    train, valid, test = kgekit.data.split_golden_set(indexes, 0.005, 0.005)
    kgekit.io.write_triples('train.txt', train)
    kgekit.io.write_triples('valid.txt', valid)
    kgekit.io.write_triples('test.txt', test)

if __name__ == '__main__':
    shuffled_split(sys.argv[1])
