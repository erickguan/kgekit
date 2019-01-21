import unittest
import kgedata
import kgekit.data
import pytest
import numpy as np
import os.path

@pytest.mark.numpyfile

def test_unwrap():
    assert kgekit.data.unpack(kgedata.TripleIndex(1, 2, 3)) == (1, 2, 3)
    assert kgekit.data.unpack(kgedata.Triple("/m/1", "/m/2", "/m/3")) == ("/m/1", "/m/2", "/m/3")

def test_transform_triple_numpy():
    np.testing.assert_equal(
        kgekit.data._transform_triple_numpy(kgedata.TripleIndex(1, 2, 3)),
        np.array([1,2,3], dtype=np.int64))

def test_pack_triples_numpy():
    np.testing.assert_equal(
        kgekit.data.pack_triples_numpy([kgedata.TripleIndex(1, 2, 3), kgedata.TripleIndex(4, 5, 6)]),
        np.array([[1,2,3], [4,5,6]], dtype=np.int64))

@pytest.fixture(scope="module")
def triple_set():
    triples, _ = kgekit.io.read_triples(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', "triple.txt"))
    return triples

@pytest.fixture(scope="module")
def duplicate_triple_set():
    triples, _ = kgekit.io.read_triples(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', "duplicate_triple.txt"))
    return triples

@pytest.fixture(scope="module")
def inverse_triple_set():
    triples, _ = kgekit.io.read_triples(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', "inverse_triple.txt"))
    return triples

def test_remove_deficit_relation(triple_set):
    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_deficit_relation(triple_set, -1)

    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_deficit_relation(triple_set, 1.1)

    assert kgekit.data.remove_deficit_relation(triple_set, 0.6) == []

    assert kgekit.data.remove_deficit_relation(triple_set, 0.4) == [
        kgedata.Triple("/m/entity2", "/country", "/m/entity3"),
        kgedata.Triple("/m/entity1", "/produced_in", "/m/entity3"),
        kgedata.Triple("/m/entity3", "/produced_in", "/m/entity4"),
        kgedata.Triple("/m/entity1", "/country", "/m/entity4")
    ]

def test_remove_near_duplicate_relation(duplicate_triple_set):
    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_near_duplicate_relation(duplicate_triple_set, -1)

    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_near_duplicate_relation(duplicate_triple_set, 1.1)

    result = kgekit.data.remove_near_duplicate_relation(duplicate_triple_set, 0.9)
    assert kgedata.Triple("/m/entity1", "/rel3", "/m/entity4") in result
    rel_set = set([t.relation for t in result])
    assert len(rel_set) == 2

def test_remove_inverse_relation(inverse_triple_set):
    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_inverse_relation(inverse_triple_set, -1)

    with pytest.raises(ValueError) as excinfo:
        kgekit.data.remove_inverse_relation(inverse_triple_set, 1.1)

    result = kgekit.data.remove_inverse_relation(inverse_triple_set, 0.9)
    assert kgedata.Triple("/m/entity1", "/rel3", "/m/entity4") in result
    rel_set = set([t.relation for t in result])
    assert len(rel_set) == 2

@pytest.fixture(scope="module")
def train_valid_test_triple_set():
    triples, _ = kgekit.io.read_triples(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'fixtures', "direct_link_triples.txt"))
    return triples[:3], [triples[3]], [triples[4]]

def test_remove_direct_link_triples(train_valid_test_triple_set):
    train, valid, test = train_valid_test_triple_set

    filtered = kgekit.data.remove_direct_link_triples(train, valid, test)
    assert filtered == [kgedata.Triple("/m/entity1", "/rel1", "/m/entity2")]
    assert valid == [kgedata.Triple("/m/entity2", "/rel4", "/m/entity3")]
    assert test == [kgedata.Triple("/m/entity4", "/rel5", "/m/entity3")]
