"""IO functions"""

from pathlib import Path

def _assert_good_file(filename):
    path = Path(filename)
    if not (path.exists() and path.is_file()):
        raise FileNotFoundError("Can't find the triple file.")

def _assert_triple_order(order):
    if frozenset(order) != frozenset(['hrt']):
        raise RuntimeError("Wrong triple order.")

def read_triple_index(filename, triple_order="hrt", delimiter='\t'):
    _assert_good_file(filename)
    _assert_triple_order(triple_order)
    print("hello")

def read_triples(filename, triple_order="hrt", delimiter='\t'):
    _assert_good_file(filename)
    _assert_triple_order(triple_order)
    pass

    vector<TripleIndex> content;
    std::ifstream fs(filename);
    for (std::string line; std::getline(fs, line); ) {
        if (auto triple = get_triple_index(line, triple_order, delimiter)) {
            content.push_back(*triple);
        } else {
            throw std::runtime_error("Can't parse file " + filename + '.');
        }
    }
