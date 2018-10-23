import sys
from collections import defaultdict
import kgekit.io
import kgekit.data
import kgekit.translation

def validate_10_relations(triples):
    entities_relation = defaultdict(set)
    for t in triples:
        h, r, t = kgekit.data.unpack(t)
        entities_relation[h].add(r)
        entities_relation[t].add(r)
    deficit_entities = []
    for k, v in entities_relation.items():
        deficit_entities.append(kgekit.translation.get_triple_from_id(k)) if len(v) < 10 else next
    num_deficits = len(deficit_entities)
    if num_deficits > 0:
        print(deficit_entities)
        print("------")
        print(entities_relation)
        raise RuntimeError(str(num_deficits) + " deficit entities found. Validation failed on some entities without sufficient relations")

def validate_reverse(triples):
    mapping = defaultdict(list)
    for t in indexes:
        h, r, t = kgekit.data.unpack(t)
        entity_pair = frozenset([h,t])
        mapping[entity_pair].append(r)
    reverse_triples = []
    for k,v in mapping.items():
        reverse_triples.append(list(k) + v) if len(v) > 1 else next
    if len(reverse_triples) > 0:
        print(reverse_triples)
        raise RuntimeError(str(len(reverse_triples)) + " reverse triples found.")


def validate(triples):
    indexer = kgekit.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    validate_10_relations(indexes)
    validate_reverse(indexes)

if __name__ == '__main__':
    triples, num_failed = kgekit.io.read_triples(sys.argv[1], "hrt", " ")
    if num_failed > 0:
        input("Failed reading " + str(num_failed) + " triple(s). Press Enter to continue...")
    validate(triples)
