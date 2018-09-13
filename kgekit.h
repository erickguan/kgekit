#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <experimental/optional>

using std::experimental::optional;
using std::vector;
using std::string;
using std::string_view;

namespace kgekit {

struct TripleIndex {
    uint32_t head;
    uint32_t relation;
    uint32_t tail;
};

struct Triple {
    string head;
    string relation;
    string tail;
};
vector<TripleIndex> read_triple_index(
    const string& filename,
    const string triple_order="hrt",
    const char sperator='\t');

optional<TripleIndex> get_triple_index(
    const string& line,
    const string& order,
    const char sperator);

} // namespace kgekit
