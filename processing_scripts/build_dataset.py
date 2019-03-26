"""Shuffle and split the data into train/valid/test sets."""

import sys
import logging
import kgekit.data
import kgekit.io
import kgedata


logging.basicConfig(level=logging.DEBUG)

def build_dataset(filename, deficit_threshold=0.00001, delimiter='\t', triple_order="hrt"):
    triples, num_failed = kgekit.io.read_triples(filename, triple_order=triple_order, delimiter=delimiter)
    if num_failed > 0:
        print("There is " + str(num_failed) + " failures.")
    logging.info("removing unqualified relations")
    triples = kgekit.data.remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold=0.97, inverse_threshold=0.97)

    logging.info("indexing triples")
    indexer = kgedata.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    logging.info("recording translations")
    kgekit.io.write_index_translation('translation.protobuf', indexer.entityIdMap(), indexer.relationIdMap())

    logging.info("build dataset")
    train, valid, test = kgekit.data.build_dataset(
        indexes,
        0.005,
        0.005)
    logging.info("writing triple files")
    kgekit.io.write_triples('train.txt', train, delimiter=delimiter, triple_order=triple_order)
    kgekit.io.write_triples('valid.txt', valid, delimiter=delimiter, triple_order=triple_order)
    kgekit.io.write_triples('test.txt', test, delimiter=delimiter, triple_order=triple_order)

if __name__ == '__main__':
    if len(sys.argv) > 3:
        build_dataset(sys.argv[1], float(sys.argv[2]))
    else:
        build_dataset(sys.argv[1])
