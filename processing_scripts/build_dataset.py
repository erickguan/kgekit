"""Shuffle and split the data into train/valid/test sets."""

import sys
import logging
import kgekit.data
import kgekit.io
import kgedata
from bidict import bidict

logging.basicConfig(level=logging.DEBUG)


def build_dataset(filename, deficit_threshold=1000, delimiter='\t', triple_order="hrt"):
    indexer = kgekit.data.Indexer()
    triples = []
    for triple in kgekit.io.yield_triples(filename, triple_order=triple_order, delimiter=delimiter):
        triples.append(indexer(triple))

    logging.info("removing unqualified relations")
    triples = kgekit.data.remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold=None, inverse_threshold=None)

    logging.info("removing unqualified entities")
    triples = kgekit.data.remove_unqualified_entities_from_triples(triples, 120)
    triples = kgekit.data.remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold=0.95, inverse_threshold=None)

    logging.info("update indexes")
    triples = indexer.shrink_indexes_in_place(triples)
    ents, rels = indexer.mappings()

    logging.info("recording translations")
    kgekit.io.write_index_translation("translation.protobuf", ents, rels)

    logging.info("build dataset")
    train, valid, test = kgekit.data.build_dataset(
        triples,
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
