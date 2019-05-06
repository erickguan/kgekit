import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

class Transform(object):
    def __init__(self, entities, lang):
        self.entities = entities
        self.lang = lang

    def __call__(self, description):
        if len(description) != 3:
            print(description)
            return True
        name, _, lang = description
        return lang != self.lang or name not in self.entities

def match_triple_descriptions(description_filename, translation_filename, lang):
    entities, _ = kgekit.io.read_translation(translation_filename)
    descriptions = kgekit.io.read_descriptions(description_filename, '\t')
    t = Transform(entities)
    filtered_descriptions = dropwhile(t, descriptions)
    filtered_descriptions = list(map(lambda l: " ".join((str(entities[l[0]]), l[1])), filtered_descriptions))
    return filtered_descriptions

if __name__ == '__main__':
    descriptions = match_triple_descriptions(sys.argv[1], sys.argv[2], sys.argv[3])
    with open('matched_descriptions.txt', 'w') as f:
        for l in descriptions:
            f.write(l + "\n")
