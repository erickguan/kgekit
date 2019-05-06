"""IO functions"""

from pathlib import Path
import kgekit.data
import kgedata
import kgekit.triple_index_pb2 as triple_pb
from kgekit.utils import assert_triple_order

DEFAULT_DELIMITER=' '

def _assert_good_file(filename):
    try:
        path = Path(filename).resolve(strict=True)
    except FileNotFoundError:
        raise FileNotFoundError("Can't find triple file at path {}".format(filename))
    except RuntimeError:
        raise FileNotFoundError("Cyclic dependency when find triple file {}".format(filename))

    if not (path.exists() and path.is_file()):
        raise FileNotFoundError("Can't find the triple file at {}.".format(path))

def yield_triples(filename, triple_order="hrt", delimiter=DEFAULT_DELIMITER, read_fn=kgedata.get_triple):
    _assert_good_file(filename)
    assert_triple_order(triple_order)
    with open(filename) as f:
        line = f.readline().rstrip('\n')
        while line:
            triple = read_fn(line, triple_order, delimiter, True)
            if triple is not None:
                yield triple
            line = f.readline().rstrip('\n')

def read_triples(filename, read_fn=kgedata.get_triple, triple_order="hrt", delimiter=DEFAULT_DELIMITER, skip_first_line=False):
    _assert_good_file(filename)
    assert_triple_order(triple_order)
    with open(filename) as f:
        if skip_first_line:
            next(f)
        indexes = [read_fn(l.rstrip('\n'), triple_order, delimiter, True) for l in f]
        filtered = list(filter(None.__ne__, indexes))
        num_none = len(indexes) - len(filtered)
        return filtered, num_none

def read_triple_indexes(filename, read_fn=kgedata.get_triple_index, triple_order="hrt", delimiter=DEFAULT_DELIMITER, skip_first_line=False):
    return read_triples(filename, read_fn, triple_order, delimiter, skip_first_line)

def read_triple_indexes_numpy(filename, triple_order="hrt", delimiter=DEFAULT_DELIMITER, skip_first_line=False):
    indexes, num_failed = read_triple_indexes(
        filename,
        triple_order=triple_order,
        delimiter=delimiter,
        skip_first_line=skip_first_line)
    return kgekit.data.pack_triples_numpy(indexes), num_failed

def _label_processing(l, delimiter):
    splits = l.rstrip().split(delimiter)
    return (splits[0], delimiter.join(splits[1:]))

# TODO: This consumes around 26G memory. It will be nicer to have stream version.
def read_labels(filename, delimiter=DEFAULT_DELIMITER):
    """read label files. Format: ent label"""
    _assert_good_file(filename)
    with open(filename) as f:
        labels = [_label_processing(l, delimiter) for l in f]
        return labels

def _label_processing(l, delimiter):
    splits = l.rstrip().split(delimiter)
    return (splits[0], delimiter.join(splits[1:]))

def _desc_processing(l, delimiter):
    splits = l.rstrip().split(delimiter)
    desc = delimiter.join(splits[1:])
    desc_splits = desc.split('@')
    return (splits[0], '@'.join(desc_splits[:-2]), desc_splits[-1])

# see `read_labels`
def read_descriptions(filename, delimiter=DEFAULT_DELIMITER):
    """read label files. Format: ent label"""
    _assert_good_file(filename)
    with open(filename) as f:
        descs = [_desc_processing(l, delimiter) for l in f]
        return descs


def write_index_translation(translation_filename, entity_ids, relation_ids):
    """write triples into a translation file."""
    translation = triple_pb.Translation()
    entities = []
    for name, index in entity_ids.items():
        translation.entities.add(element=name, index=index)
    relations = []
    for name, index in relation_ids.items():
        translation.relations.add(element=name, index=index)
    with open(translation_filename, "wb") as f:
        f.write(translation.SerializeToString())

def write_triples(filename, triples, delimiter=DEFAULT_DELIMITER, triple_order="hrt"):
    """write triples to file."""
    with open(filename, 'w') as f:
        for t in triples:
            line = t.serialize(delimiter, triple_order)
            f.write(line + "\n")

def read_translation(filename):
    """Returns protobuf mapcontainer. Read from translation file."""
    translation = triple_pb.Translation()
    with open(filename, "rb") as f:
        translation.ParseFromString(f.read())

    def unwrap_translation_units(units):
        for u in units: yield u.element, u.index

    return (list(unwrap_translation_units(translation.entities)),
        list(unwrap_translation_units(translation.relations)))

def read_openke_translation(filename, delimiter='\t', entity_first=True):
    """Returns map with entity or relations from plain text."""
    result = {}
    with open(filename, "r") as f:
        _ = next(f) # pass the total entry number
        for line in f:
            line_slice = line.rstrip().split(delimiter)
            if not entity_first:
                line_slice = list(reversed(line_slice))
            result[line_slice[0]] = line_slice[1]

    return result
