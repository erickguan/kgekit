#pragma once

#include <string>

#include <fmt/format.h>

namespace kgedata {

namespace detail {

void assert_triple_order(const std::string& order);

constexpr auto kNumTripleElements = 3;
constexpr auto kTripleRelationOffestInABatch = 1; // h, r, t
constexpr auto kTripleHeadOffestInABatch = 0;
constexpr auto kTripleTailOffestInABatch = 2;

} // namespace detail

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
    std::string repr() const
    {
        return fmt::format("({0}, {1}, {2})", head, relation, tail);
    }
    std::string serialize(const std::string& delimiter, const std::string& order) const
    {
        detail::assert_triple_order(order);
        // TODO: fix this
        if (order == "htr") {
            return fmt::format("{0}{3}{1}{3}{2}", head, tail, relation, delimiter);
        }
        return fmt::format("{0}{3}{1}{3}{2}", head, relation, tail, delimiter);
    }
};

struct Triple {
    std::string head;
    std::string relation;
    std::string tail;
    Triple() = default;
    Triple(const std::string& h, const std::string& r, const std::string& t) : head(h), relation(r), tail(t) {}
    bool operator==(const Triple& rhs) const
    {
        return rhs.head == head && rhs.relation == relation && rhs.tail == tail;
    }
    std::string repr() const
    {
        return fmt::format("({0}, {1}, {2})", head, relation, tail);
    }
    std::string serialize(const std::string& delimiter, const std::string& order) const
    {
        detail::assert_triple_order(order);
        // TODO: fix this
        if (order == "htr") {
            return fmt::format("{0}{3}{1}{3}{2}", head, tail, relation, delimiter);
        }
        return fmt::format("{0}{3}{1}{3}{2}", head, relation, tail, delimiter);
    }
};

} // namesapce kgedata
