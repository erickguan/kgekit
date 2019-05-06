import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile

def gen_plaintext_translation(translation_filename):
    entities, relations = kgekit.io.read_translation(translation_filename)
    with open('entities.txt', 'w') as f:
        for element, index in entities:
            f.write(f"{element}\t{index}\n")

    with open('relations.txt', 'w') as f:
        for element, index in relations:
            f.write(f"{element}\t{index}\n")


if __name__ == '__main__':
    gen_plaintext_translation(sys.argv[1])