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

    def shrink_indexes_in_place(self, triples):
        """Uses a union find to find segment."""

        ent_flags = [False for i in range(self._ent_id)]
        rel_flags = [False for i in range(self._rel_id)]

        for t in triples:
            ent_flags[t.head] = True
            ent_flags[t.tail] = True
            rel_flags[t.relation] = True

        ents = bidict()
        ent_idx = 0
        for previous_idx, ent_exist in enumerate(ent_flags):
            if ent_exist:
                ents[self._ents.inverse[previous_idx]] = ent_idx
                ent_idx += 1
        logging.info(f"before shrinking: {self._ent_id}\nafter shrinking: {ent_idx}")

        rels = bidict()
        rel_idx = 0
        for previous_idx, rel_exist in enumerate(rel_flags):
            if rel_exist:
                rels[self._rels.inverse[previous_idx]] = rel_idx
                rel_idx += 1
        logging.info(f"before shrinking: {self._rel_id}\nafter shrinking: {rel_idx}")
        
        new_triples = [kgedata.TripleIndex(ents[self._ents.inverse[t.head]], rels[self._rels.inverse[t.relation]], ents[self._ents.inverse[t.tail]]) for t in triples]

        self._ent_id = ent_idx
        self._ents = ents

        self._rel_id = rel_idx
        self._rels = rels
        
        return new_triples


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

def _generate_triple_from_list(triples):
    for t in triples:
        yield t.head
        yield t.relation
        yield t.tail

def pack_triples_numpy(triples):
    """Packs a list of triple indexes into a 2D numpy array."""
    return np.fromiter(_generate_triple_from_list(triples), dtype=np.dtype(np.int64), count=len(triples)*3).reshape(-1, 3)

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

def remove_deficit_entities(triples, threshold):
    counter = defaultdict(int)
    for t in triples:
        counter[t.head] += 1
        counter[t.tail] += 1

    num_triples = len(triples)
    removal_set = set()
    for ent, ent_items in counter.items():
        if ent_items < threshold:
            removal_set.add(ent)
    logging.info("Removing {} entities".format(len(removal_set)))

    return list(filterfalse(lambda x: x.head in removal_set or x.tail in removal_set, triples))
    

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

def remove_unqualified_relations_from_triples(triples, deficit_threshold, duplicate_threshold=None, inverse_threshold=None):
    triples = remove_deficit_relation(triples, deficit_threshold)
    if duplicate_threshold is not None:
        triples = remove_near_duplicate_relation(triples, duplicate_threshold)
    if inverse_threshold is not None:
        triples = remove_inverse_relation(triples, inverse_threshold)
    return triples

def remove_unqualified_entities_from_triples(triples, deficit_threshold=10):
    triples = remove_deficit_entities(triples, deficit_threshold)
    return triples

def build_dataset(triples, valid_ratio, test_ratio):
    triples = list(shuffle_triples(triples))
    train, valid, test = split_golden_set(triples, valid_ratio, test_ratio)
    train = remove_direct_link_triples(train, valid, test)
    return train, valid, test
