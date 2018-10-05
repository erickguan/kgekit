#pragma once

#include <string>
#include <vector>
#include <experimental/filesystem>

#if defined(__clang__) && !defined(__apple_build_version__) && __clang_major__ < 7
#include <experimental/string_view>
#include <experimental/optional>
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ <= 9
#include <experimental/string_view>
#include <experimental/optional>
#else
#include <string_view>
#include <optional>
#endif

namespace kgekit {

using std::string;
using std::vector;

#if defined(__clang__) && !defined(__apple_build_version__) && __clang_major__ < 7
using std::experimental::optional;
using std::experimental::make_optional;
using std::experimental::string_view;
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ <= 9
using std::experimental::optional;
using std::experimental::make_optional;
using std::experimental::string_view;
#else
using std::optional;
using std::make_optional;
using std::string_view;
#endif

namespace internal {

namespace fs = std::experimental::filesystem;

void assert_good_file(fs::path filename);
void assert_triple_order(const string& order);

} // namespace internal

struct TripleIndex {
    uint32_t head;
    uint32_t relation;
    uint32_t tail;
    bool operator==(const TripleIndex& rhs) const
    {
        return rhs.head == head && rhs.relation == relation && rhs.tail == tail;
    }
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
