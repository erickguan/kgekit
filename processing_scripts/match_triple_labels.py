import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

def match_triple_labels(triple_filename):
    triples, _ = kgekit.io.read_triples(triple_filename, 'hrt', ' ')
    chained_triple = chain([(t.head, t.tail) for t in triples])
    entities = frozenset(chained_triple)
    labels = kgekit.io.read_labels(triple_filename, ' ')
    filtered_labels = list(dropwhile(lambda x: x[0] not in entities, labels))
    return filtered_labels

if __name__ == '__main__':
    labels = match_triple_labels(sys.argv[1])
    with open('matched_labels.txt', 'w') as f:
        for l in labels:
            f.write(" ".join(l) + "\n")
