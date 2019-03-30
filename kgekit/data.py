"""data utils function"""

from collections import defaultdict
from itertools import filterfalse
from functools import reduce
import numpy as np
import logging
import kgedata
from superminhash import Superminhash
from bidict import bidict


class Indexer(object):
    """An indexer for triples."""

    def __init__(self):
        self._ents = bidict()
        self._rels = bidict()
        self._ent_id = 0
        self._rel_id = 0

    def __call__(self, t):
        for e in (t.head, t.tail):
            if e not in self._ents:
                self._ents[e] = self._ent_id
                self._ent_id += 1
        if t.relation not in self._rels:
            self._rels[t.relation] = self._rel_id
            self._rel_id += 1

        return kgedata.TripleIndex(self._ents[t.head], self._rels[t.relation], self._ents[t.tail])

    def mappings(self):
        return self._ents, self._rels

    class UnionFind(object):
        def __init__(self, max_id):
            """every elements has a root of its precedence except the first."""
            self._root = [0] + [i for i in range(max_id-1)]
            self._max_id = max_id

        def add(self, e):
            """Create new root"""
            self._root[e] = e

        def root(self, e):
            return e == self._root[e]

        def roots(self):
            [self.root(e) for e in range(self._max_id)]

        def union(self, e1, e2):
            """union by the smallest root"""
            root1 = self.find(e1)
            root2 = self.find(e2)
            if root1 == root2: return
            if root1 < root2:
                self._root[root2] = root1
            else:
                self._root[root1] = root2

        def find(self, e):
            """uses path compression."""
            if e <= 0: return 0
            if e != self._root[e]:
                self._root[e] = self.find(self._root[e])
            return self._root[e]

    def shrink_indexes_in_place(self, triples):
        """Uses a union find to find segment."""

        _ent_roots = UnionFind(self._ent_id)
        _rel_roots = UnionFind(self._rel_id)

        for t in triples:
            _ent_roots.add(t.head)
            _ent_roots.add(t.tail)
            _rel_roots.add(t.relation)

        for i, t in enumerate(triples):
            h = _ent_roots.find(t.head)
            r = _rel_roots.find(t.relation)
            t = _ent_roots.find(t.tail)
            triples[i] = kgedata.TripleIndex(h, r, t)

        ents = bidict()
        available_ent_idx = 0
        for previous_idx, ent_exist in enumerate(_ent_roots.roots()):
            if not ent_exist:
                self._ents.inverse.pop(previous_idx)
            else:
                ents[self._ents.inverse[previous_idx]] = available_ent_idx
            available_ent_idx += 1
        rels = bidict()
        available_rel_idx = 0
        for previous_idx, rel_exist in enumerate(_rel_roots.roots()):
            if not rel_exist:
                self._rels.inverse.pop(previous_idx)
            else:
                rels[self._rels.inverse[previous_idx]] = available_rel_idx
            available_rel_idx += 1
        self._ents = ents
        self._rels = rels
        self._ent_id = available_ent_idx
        self._rel_id = available_rel_idx


def build_index_and_mapping(triples):
    """index all triples into indexes and return their mappings"""
    ents = bidict()
    rels = bidict()
    ent_id = 0
    rel_id = 0

    collected = []
    for t in triples:
        for e in (t.head, t.tail):
            if e not in ents:
                ents[e] = ent_id
                ent_id += 1
        if t.relation not in rels:
            rels[t.relation] = rel_id
            rel_id += 1
        collected.append(kgedata.TripleIndex(ents[t.head], rels[t.relation], ents[t.tail]))

    return collected, ents, rels


def recover_triples_from_mapping(indexes, ents: bidict, rels: bidict):
    """recover triples from mapping."""
    triples = []
    for t in indexes:
        triples.append(kgedata.Triple(ents.inverse[t.head], rels.inverse[t.relation], ents.inverse[t.tail]))
    return triples


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
    return a.similarity(b) > threshold

def _assert_threshold(threshold):
    if threshold <= 0.0 or threshold > 1.0: raise ValueError("The threshold {} is not in the valid range of (0, 1]")

def remove_deficit_relation(triples, threshold=1000):
    logging.debug("remove deficit relation")

    counter = defaultdict(int)
    for t in triples:
        counter[t.relation] += 1

    num_triples = len(triples)
    removal_set = set()
    for rel, rel_items in counter.items():
        if rel_items < threshold:
            removal_set.add(rel)
    logging.info("Removing {} relations: {}".format(len(removal_set), str(removal_set)))

    return list(filterfalse(lambda x: x.relation in removal_set, triples))

def remove_near_duplicate_relation(triples, threshold=0.97):
    """If entity pairs in a relation is as close as another relations, only keep one relation of such set."""
    logging.debug("remove duplicate")

    _assert_threshold(threshold)

    duplicate_rel_counter = defaultdict(list)
    relations = set()
    for t in triples:
        duplicate_rel_counter[t.relation].append(f"{t.head} {t.tail}")
        relations.add(t.relation)
    relations = list(relations)

    num_triples = len(triples)
    removal_relation_set = set()

    for rel, values in duplicate_rel_counter.items():
        duplicate_rel_counter[rel] = Superminhash(values)
    for i in relations:
        for j in relations:
            if i == j or i in removal_relation_set or j in removal_relation_set: continue
            close_relations = [i]
            if _set_close_to(duplicate_rel_counter[i], duplicate_rel_counter[j], threshold):
                close_relations.append(j)
        if len(close_relations) > 1:
            close_relations.pop(np.random.randint(len(close_relations)))
            removal_relation_set |= set(close_relations)
    logging.info("Removing {} relations: {}".format(len(removal_relation_set), str(removal_relation_set)))

    return list(filterfalse(lambda x: x.relation in removal_relation_set, triples))

def remove_inverse_relation(triples, threshold=0.97):
    """if entity pairs in a relation overlaps its reverse pairs in another relation, only keep one relation of such set."""
    logging.debug("remove inverse")
    _assert_threshold(threshold)

    rel_counter = defaultdict(list)
    inverse_rel_counter = defaultdict(list)
    relations = set()
    for t in triples:
        rel_counter[t.relation].append(f"{t.head} {t.tail}")
        inverse_rel_counter[t.relation].append(f"{t.tail} {t.head}")
        relations.add(t.relation)
    relations = list(relations)

    for rel, values in rel_counter.items():
        rel_counter[rel] = Superminhash(values)
    for rel, values in inverse_rel_counter.items():
        inverse_rel_counter[rel] = Superminhash(values)

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
    logging.info("Removing {} relations: {}".format(len(removal_relation_set), str(removal_relation_set)))

    return list(filterfalse(lambda x: x.relation in removal_relation_set, triples))

def remove_direct_link_triples(train, valid, test):
    """Remove direct links in the training sets."""
    pairs = set()
    merged = valid + test
    for t in merged:
        pairs.add((t.head, t.tail))

    filtered = filterfalse(lambda t: (t.head, t.tail) in pairs or (t.tail, t.head) in pairs, train)
    return list(filtered)

def remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold, inverse_threshold):
    triples = remove_deficit_relation(triples, deficit_threshold)
    triples = remove_near_duplicate_relation(triples, duplicate_threshold)
    triples = remove_inverse_relation(triples, inverse_threshold)
    logging.info("current triples: " + str(len(triples)))
    return triples

def build_dataset(triples, valid_ratio, test_ratio):
    triples = list(shuffle_triples(triples))
    train, valid, test = split_golden_set(triples, valid_ratio, test_ratio)
    train = remove_direct_link_triples(train, valid, test)
    return train, valid, test
