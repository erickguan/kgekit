#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <experimental/filesystem>

namespace kgekit {

using std::optional;
using std::vector;
using std::string;
using std::string_view;

namespace internal {

namespace fs = std::experimental::filesystem;

void assert_good_file(fs::path filename);
void assert_triple_order(const string& order);

} // namespace internal

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
    const char delimiter='\t');

optional<TripleIndex> get_triple_index(
    const string& line,
    const string& order,
    const char delimiter);

} // namespace kgekit
