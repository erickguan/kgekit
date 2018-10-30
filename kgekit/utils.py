"Utilties functions"

def assert_triple_order(order):
    if frozenset(order) != frozenset('hrt'):
        raise RuntimeError("Wrong triple order.")
