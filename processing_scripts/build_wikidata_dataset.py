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
    unused_properties = set(['P3819', 'P4118', 'P4142', 'P6205', 'P6729', 'P1022', 'P1052', 'P1271', 'P2158', 'P2524', 'P2536', 'P3480', 'P3978', 'P4121', 'P4165', 'P4280', 'P4294', 'P4393', 'P4487', 'P4512', 'P4734', 'P4752', 'P5231', 'P5687', 'P6630', 'P6727', 'P849', 'P1738', 'P2086', 'P2480', 'P3199', 'P3717', 'P3727', 'P3731', 'P4018', 'P4113', 'P4221', 'P4355', 'P4597', 'P4833', 'P4926', ])
    for triple in kgekit.io.yield_triples(filename, triple_order=triple_order, delimiter=delimiter):
        triples.append(indexer(triple))

    logging.info("removing unqualified relations")
    triples = kgekit.data.remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold=None, inverse_threshold=None)

    logging.info("removing unqualified entities")
    triples = kgekit.data.remove_unqualified_entities_from_triples(triples, 16)

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
