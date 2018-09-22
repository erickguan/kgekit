import sys
import rdflib

g = rdflib.Graph()
g.load(sys.argv[1])

i = 0
for s, p, o in g:
    if i > 100:
        exit()
    print(s, p, o)
    i += 1
