#pragma once

#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <experimental/filesystem>

#if __cplusplus < 201703L
#define EXPERIMENTAL_CPP17
#endif

#ifdef EXPERIMENTAL_CPP17
#include <experimental/string_view>
#include <experimental/optional>
#else
#include <string_view>
#include <optional>
#endif

#include <fmt/format.h>

namespace kgekit {

using std::string;
using std::vector;
using std::array;

#ifdef EXPERIMENTAL_CPP17
using std::experimental::optional;
using std::experimental::make_optional;
using std::experimental::nullopt_t;
using std::experimental::string_view;
#else
using std::optional;
using std::nullopt_t;
using std::make_optional;
using std::string_view;
#endif


namespace internal {

void assert_triple_order(const string& order);
const auto kBufferSize = 128;

/*
 * Pack two int64_t values into a int64_t.
 * Here assumes parameters are no larger than int32_t.
 * The reason is that int128_t is not supported and time is limited.
 */
inline int64_t _pack_value(int64_t a, int64_t b)
{
    if (a > INT32_MAX || b > INT32_MAX) {
        throw std::runtime_error("Entry ID exceeds INT32_MAX");
    }
    return (static_cast<int64_t>(a) << 32) + b;
}


} // namespace internal

/*
 * Low level data structure are smaller to manuplate in C++.
 * So they are exported to Python. They have to be concreate so we can bind them.
 */
struct TripleIndex {
    int32_t head = -1;
    int32_t relation = -1;
    int32_t tail = -1;
    TripleIndex() = default;
    TripleIndex(int32_t h, int32_t r, int32_t t) : head(h), relation(r), tail(t) {}
    bool operator==(const TripleIndex& rhs) const
    {
        return rhs.head == head && rhs.relation == relation && rhs.tail == tail;
    }
    string repr() const
    {
        return fmt::format("({0}, {1}, {2})", head, relation, tail);
    }
    string serialize(const string& delimiter) const
    {
        return fmt::format("{0}{3}{1}{3}{2}", head, relation, tail, delimiter);
    }
};

struct Triple {
    string head;
    string relation;
    string tail;
    Triple() = default;
    Triple(const string& h, const string& r, const string& t) : head(h), relation(r), tail(t) {}
    bool operator==(const Triple& rhs) const
    {
        return rhs.head == head && rhs.relation == relation && rhs.tail == tail;
    }
    string repr() const
    {
        return fmt::format("({0}, {1}, {2})", head, relation, tail);
    }
    string serialize(const string& delimiter) const
    {
        return fmt::format("{0}{3}{1}{3}{2}", head, relation, tail, delimiter);
    }
};

vector<TripleIndex> read_triple_index(
    const string& filename,
    const string triple_order="hrt",
    const char delimiter='\t');

optional<TripleIndex> get_triple_index(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order = false);

optional<Triple> get_triple(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order = false);


} // namespace kgekit
