"""data utils function"""

from collections import defaultdict
from itertools import filterfalse
from functools import reduce
import numpy as np

def unpack(triple):
    """unpacks triple into (h, r, t). Can take Triple or TripleIndex."""
    return triple.head, triple.relation, triple.tail

def shuffle_triples(triples):
    """Shuffle triples."""
    return np.random.permutation(triples)

def split_golden_set(triples, valid_ratio, test_ratio):
    """Split the data into train/valid/test sets."""
    assert valid_ratio >= 0.0
    assert test_ratio >= 0.0
    num_valid = int(len(triples) * valid_ratio)
    num_test = int(len(triples) * test_ratio)
    valid_set = triples[:num_valid]
    test_set = triples[num_valid:num_valid+num_test]
    train_set = triples[num_valid+num_test:]
    assert len(valid_set) + len(test_set) + len(train_set) == len(triples)

    return train_set, valid_set, test_set

def _transform_triple_numpy(x):
    """Transform triple index into a 1-D numpy array."""
    return np.array([x.head, x.relation, x.tail], dtype=np.int64)

def pack_triples_numpy(triples):
    """Packs a list of triple indexes into a 2D numpy array."""
    if len(triples) == 0:
        return np.array([], dtype=np.int64)
    return np.stack(list(map(_transform_triple_numpy, triples)), axis=0)

def _set_close_to(a, b, threshold):
    return len(a & b) / len(b) > threshold

def _assert_threshold(threshold):
    if threshold <= 0.0 or threshold > 1.0: raise ValueError("The threshold {} is not in the valid range of (0, 1]")

def remove_deficit_relation(triples, threshold=0.0001):
    _assert_threshold(threshold)

    counter = defaultdict(list)
    for t in triples:
        counter[t.relation].append(t)

    num_triples = len(triples)
    removal_set = set()
    for rel, rel_items in counter.items():
        if len(rel_items) / num_triples < threshold:
            removal_set = removal_set | set(rel_items)

    return list(filterfalse(lambda x: x in removal_set, triples))

def remove_near_duplicate_relation(triples, threshold=0.97):
    """If entity pairs in a relation is as close as another relations, only keep one relation of such set."""
    _assert_threshold(threshold)

    duplicate_rel_counter = defaultdict(set)
    relations = set()
    for t in triples:
        duplicate_rel_counter[t.relation].add((t.head, t.tail))
        relations.add(t.relation)
    relations = list(relations)

    num_triples = len(triples)
    removal_relation_set = set()
    for i in relations:
        if i in removal_relation_set: continue
        close_relations = [i]
        for j in relations:
            if i == j or j in removal_relation_set: continue
            if _set_close_to(duplicate_rel_counter[i], duplicate_rel_counter[j], threshold):
                close_relations.append(j)
        if len(close_relations) > 1:
            close_relations.pop(np.random.randint(len(close_relations)))
            removal_relation_set |= set(close_relations)

    return list(filterfalse(lambda x: x.relation in removal_relation_set, triples))

def remove_inverse_relation(triples, threshold=0.97):
    """if entity pairs in a relation overlaps its reverse pairs in another relation, only keep one relation of such set."""
    _assert_threshold(threshold)

    rel_counter = defaultdict(set)
    inverse_rel_counter = defaultdict(set)
    relations = set()
    for t in triples:
        rel_counter[t.relation].add((t.head, t.tail))
        inverse_rel_counter[t.relation].add((t.tail, t.head))
        relations.add(t.relation)
    relations = list(relations)

    num_triples = len(triples)
    removal_relation_set = set()
    for i in relations:
        if i in removal_relation_set: continue
        close_relations = [i]
        for j in relations:
            if i == j or j in removal_relation_set: continue
            if _set_close_to(rel_counter[i], inverse_rel_counter[j], threshold):
                close_relations.append(j)
        if len(close_relations) > 1:
            close_relations.pop(np.random.randint(len(close_relations)))
            removal_relation_set |= set(close_relations)

    return list(filterfalse(lambda x: x.relation in removal_relation_set, triples))

def remove_direct_link_triples(train, valid, test):
    """Remove direct links in the training sets."""
    pairs = set()
    merged = valid + test
    for t in merged:
        pairs.add((t.head, t.tail))

    filtered = filterfalse(lambda t: (t.head, t.tail) in pairs or (t.tail, t.head) in pairs, train)
    return list(filtered)

def build_dataset(triples, valid_ratio, test_ratio, deficit_threshold, duplicate_threshold, inverse_threshold):
    triples = remove_deficit_relation(triples, deficit_threshold)
    triples = remove_near_duplicate_relation(triples, duplicate_threshold)
    triples = remove_inverse_relation(triples, inverse_threshold)

    triples = shuffle_triples(triples)
    train, valid, test = split_golden_set(triples)
    train = remove_direct_link_triples(train, valid, test)
    return train, valid, test
