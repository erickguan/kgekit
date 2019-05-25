import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import random
from itertools import chain, dropwhile
import pandas

def match_selected_triples(selected_filename, summary_filename):
    selected = {}
    with open(selected_filename, "r") as f:
        for l in f:
            l = l.rstrip().lstrip('Q')
            selected[int(l)] = None

    a = pandas.read_csv(summary_filename, sep='\t', quotechar='"', error_bad_lines=False)
    print(a.head(10))

if __name__ == '__main__':
    labels = match_selected_triples(sys.argv[1], sys.argv[2])
