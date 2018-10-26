import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

def match_triple_labels(triple_filename, label_filename):
    triples, _ = kgekit.io.read_triples(triple_filename, 'hrt', ' ')
    chained_triple = chain([(t.head, t.tail) for t in triples])
    entities = frozenset(chained_triple)
    labels = kgekit.io.read_labels(label_filename, ' ')
    filtered_labels = list(dropwhile(lambda x: x in entities, labels))
    return triples, filtered_labels

if __name__ == '__main__':
    match_triple_labels(sys.argv[1], sys.argv[2])
