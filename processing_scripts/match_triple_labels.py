import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

def match_triple_labels(label_filename, translation_filename):
    entities, _ = kgekit.io.read_translation(translation_filename)
    labels = kgekit.io.read_labels(label_filename, ' ')
    filtered_labels = dropwhile(lambda x: x[0] not in entities or len(x) != 2, labels)
    filtered_labels = list(map(lambda l: (entities[l[0]], l[1] ), filtered_labels))
    return filtered_labels

if __name__ == '__main__':
    labels = match_triple_labels(sys.argv[1], sys.argv[2])
    with open('matched_labels.txt', 'w') as f:
        for l in labels:
            f.write(" ".join(l) + "\n")
