"""data utils function"""

def unwrap(triple):
    """unwraps triple into (h, r, t). Can take Triple or TripleIndex."""
    return triple.head, triple.relation, triple.tail
