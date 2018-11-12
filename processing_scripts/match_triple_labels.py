import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

class Transform(object):
    def __init__(self, entities):
        self.entities = entities

    def __call__(self, label):
        if len(label) != 2:
            print(label)
            return True
        name, _ = label
        return name not in self.entities

def match_triple_labels(label_filename, translation_filename):
    entities, _ = kgekit.io.read_translation(translation_filename)
    labels = kgekit.io.read_labels(label_filename, ' ')
    t = Transform(entities)
    filtered_labels = dropwhile(t, labels)
    filtered_labels = list(map(lambda l: " ".join((str(entities[l[0]]), l[1])), filtered_labels))
    return filtered_labels

if __name__ == '__main__':
    labels = match_triple_labels(sys.argv[1], sys.argv[2])
    with open('matched_labels.txt', 'w') as f:
        for l in labels:
            f.write(l + "\n")
