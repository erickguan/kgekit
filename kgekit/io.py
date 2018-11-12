"""IO functions"""

from pathlib import Path
import kgekit
import kgekit.triple_index_pb2 as triple_pb
from kgekit.utils import assert_triple_order

DEFAULT_DELIMITER=' '

def _assert_good_file(filename):
    path = Path(filename)
    if not (path.exists() and path.is_file()):
        raise FileNotFoundError("Can't find the triple file.")

def read_triples(filename, triple_order="hrt", delimiter=DEFAULT_DELIMITER, read_fn=kgekit.get_triple):
    _assert_good_file(filename)
    assert_triple_order(triple_order)
    with open(filename) as f:
        indexes = [read_fn(l.rstrip('\n'), triple_order, delimiter, True) for l in f]
        filtered = list(filter(None.__ne__, indexes))
        num_none = len(indexes) - len(filtered)
        return filtered, num_none

def read_triple_indexes(filename, triple_order="hrt", delimiter=DEFAULT_DELIMITER, read_fn=kgekit.get_triple_index):
    return read_triples(filename, triple_order, delimiter, read_fn)

def _label_processing(l, delimiter):
    splits = l.rstrip().split(delimiter)
    return (splits[0], delimiter.join(splits[1:]))

def read_labels(filename, delimiter=DEFAULT_DELIMITER):
    """read label files. Format: ent label"""
    _assert_good_file(filename)
    with open(filename) as f:
        labels = [_label_processing(l, delimiter) for l in f]
        return labels

def write_index_translation(translation_filename, entity_ids, relation_ids, delimiter=DEFAULT_DELIMITER):
    """write triples into a translation file."""
    translation = triple_pb.Translation()
    entities = []
    for name, index in entity_ids.items():
        translation.entities[name] = index
    relations = []
    for name, index in relation_ids.items():
        translation.relations[name] = index
    with open(translation_filename, "wb") as f:
        f.write(translation.SerializeToString())

def write_triples(filename, triples, delimiter=DEFAULT_DELIMITER):
    """write triples to file."""
    with open(filename, 'w') as f:
        for t in triples:
            line = t.serialize(delimiter)
            f.write(line + "\n")

def read_translation(filename):
    """Returns protobuf mapcontainer. Read from translation file."""
    translation = triple_pb.Translation()
    with open(filename, "rb") as f:
        translation.ParseFromString(f.read())
    return (translation.entities, translation.relations)
