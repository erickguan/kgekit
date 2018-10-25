import sys
from collections import defaultdict
import kgekit.io
import kgekit.data

def _get_ent_rels(triples):
    entities_relation = defaultdict(set)
    for triple in triples:
        h, r, t = kgekit.data.unpack(triple)
        entities_relation[h].add((t,r))
        entities_relation[t].add((h,r))
    return entities_relation

def _get_deficit_ents(triples):
    ent_rels = _get_ent_rels(triples)
    deficit_entities = []
    for k, v in ent_rels.items():
        deficit_entities.append(k) if len(v) < 10 else next
    return frozenset(deficit_entities)


def validate_10_relations(triples, getEntityFromId):
    entities_relation = _get_ent_rels(triples)
    deficit_entities = []
    for k, v in entities_relation.items():
        deficit_entities.append(getEntityFromId(k)) if len(v) < 10 else next
    num_deficits = len(deficit_entities)
    if num_deficits > 0:
        print(deficit_entities)
        print("------")
        print(entities_relation)
        raise RuntimeError(str(num_deficits) + " deficit entities found. Validation failed on some entities without sufficient relations")

def validate_reverse(indexes, getEntityFromId, getRelationFromId):
    mapping = defaultdict(list)
    for triple in indexes:
        h, r, t = kgekit.data.unpack(triple)
        entity_pair = frozenset([h,t])
        mapping[entity_pair].append((h,r,t))
    reverse_triples = []
    for k,v in mapping.items():
        reverse_triples.append(v) if len(v) > 1 else next
    if len(reverse_triples) > 0:
        print(reverse_triples)
        raise RuntimeError(str(len(reverse_triples)) + " reverse triples found.")


def validate(triples):
    indexer = kgekit.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    validate_10_relations(indexes, indexer.getEntityFromId)
    validate_reverse(indexes, indexer.getEntityFromId, indexer.getRelationFromId)

def remove10(filename, out_filename):
    seperator = " "
    triples, num_failed = kgekit.io.read_triples(filename, "hrt", seperator)
    if num_failed > 0:
        input("Failed reading " + str(num_failed) + " triple(s). Press Enter to continue...")
    indexer = kgekit.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    ents = _get_deficit_ents(indexes)
    removed_ents = len(ents)
    original = set(indexes)
    toDeleted = set()
    for triple in indexes:
        h, r, t = kgekit.data.unpack(triple)
        if h in ents or t in ents:
            toDeleted.add(triple)
    new_set = original - toDeleted
    removed_triples = len(original) - len(new_set)

    print("Removed entities " + str(removed_ents))
    print("Removed triples " + str(removed_triples))
    with open(out_filename, 'w') as f:
        for idx in new_set:
            f.write(indexer.getEntityFromId(idx.head) + seperator + indexer.getRelationFromId(idx.relation) + seperator + indexer.getEntityFromId(idx.tail) + "\n")


if __name__ == '__main__':
    if 'validate' == sys.argv[1]:
        triples, num_failed = kgekit.io.read_triples(sys.argv[2], "hrt", " ")
        if num_failed > 0:
            input("Failed reading " + str(num_failed) + " triple(s). Press Enter to continue...")
        validate(triples)
    if 'remove_10' == sys.argv[1]:
        remove10(sys.argv[2], sys.argv[3])