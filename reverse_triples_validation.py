import sys
import kgekit.io

def validate(triples):
    indexer = kgekit.EntityNumberIndexer(triples, "hrt")
    indexes = indexer.indexes()
    mapping = {}
    for t in indexes:
        h, r, t = t.head, t.relation, t.tail
        if h < t:
            k = (h,t)
        else:
            k = (t,h)
        mapping.setdefault(k, list())
        mapping[k].append(r)
    for k,v in mapping.items():
        if len(v) > 1:
            print(k,v)

if __name__ == '__main__':
    validate(kgekit.io.read_triples(sys.argv[1], "hrt", " "))
