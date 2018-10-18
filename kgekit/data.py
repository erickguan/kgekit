"""data utils function"""

def unpack(triple):
    """unpacks triple into (h, r, t). Can take Triple or TripleIndex."""
    return triple.head, triple.relation, triple.tail
