"""IO functions"""

from pathlib import Path
import kgekit

def _assert_good_file(filename):
    path = Path(filename)
    if not (path.exists() and path.is_file()):
        raise FileNotFoundError("Can't find the triple file.")

def _assert_triple_order(order):
    if frozenset(order) != frozenset('hrt'):
        raise RuntimeError("Wrong triple order.")

def read_triple_indexes(filename, triple_order="hrt", delimiter='\t'):
    _assert_good_file(filename)
    _assert_triple_order(triple_order)
    with open(filename) as f:
        filtered = kgekit.TripleIndexList()
        num_none = 0
        for l in f:
            t = kgekit.get_triple(l.rstrip('\n'), triple_order, delimiter, True)
            if t is None:
                num_none += 1
            else:
                filtered.append(t)
        return filtered, num_none

def read_triples(filename, triple_order="hrt", delimiter='\t'):
    _assert_good_file(filename)
    _assert_triple_order(triple_order)
    with open(filename) as f:
        indexes = [kgekit.get_triple(l.rstrip('\n'), triple_order, delimiter, True) for l in f]
        print(indexes)
        filtered = list(filter(None.__ne__, indexes))
        num_none = len(indexes) - len(filtered)
        return filtered, num_none

