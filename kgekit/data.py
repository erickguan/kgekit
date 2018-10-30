"""data utils function"""

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


