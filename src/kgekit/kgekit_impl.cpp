#include "kgekit.h"

#include <vector>
#include <unordered_set>
#include <range/v3/all.hpp>
#include <range/v3/view/split.hpp>

namespace kgekit {

using std::vector;

namespace internal {

void assert_triple_order(const string& order)
{
    std::unordered_set<char> order_set(order.cbegin(), order.cend());
    if (order_set.empty() || order_set.size() != order.size()) {
        throw std::invalid_argument("Order is not valid. There is duplicate member.");
    }
    if (order_set.find('h') == order_set.end() ||
        order_set.find('r') == order_set.end() ||
        order_set.find('t') == order_set.end() ||
        order_set.size() != 3) {
        throw std::invalid_argument("Order is not valid. It's not a valid triple shape.");
    }
}
} // namespace internal

optional<TripleIndex> get_triple_index(const string& line, const string& order, const char delimiter, bool skip_checking_order)
{
    if (!skip_checking_order) {
        try {
            internal::assert_triple_order(order);
        } catch (const std::invalid_argument& e) {
            return {};
        }
    }
    decltype(line.find(delimiter)) last_pos = 0;
    TripleIndex triple;

    vector<string> view = line | ranges::view::split(delimiter);

    for (auto i = 0; i < order.size(); ++i) {
        if (i >= view.size()) { return {}; }
        uint32_t idx = static_cast<uint32_t>(std::stoul(view[i]));

        switch (order[i]) {
        case 'h':
            triple.head = idx;
            break;
        case 'r':
            triple.relation = idx;
            break;
        case 't':
            triple.tail = idx;
            break;
        default:
            break;
        }
    }
    return { triple };
}


optional<Triple> get_triple(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order)
{
    if (!skip_checking_order) {
        try {
            internal::assert_triple_order(order);
        } catch (const std::invalid_argument& e) {
            return {};
        }
    }
    decltype(line.find(delimiter)) last_pos = 0;
    Triple triple;

    vector<string> view = line | ranges::view::split(delimiter);

    for (auto i = 0; i < order.size(); ++i) {
        if (i >= view.size()) { return {}; }

        auto& idx = view[i];
        switch (order[i]) {
        case 'h':
            triple.head = idx;
            break;
        case 'r':
            triple.relation = idx;
            break;
        case 't':
            triple.tail = idx;
            break;
        default:
            break;
        }
    }
    return { triple };
}

} // namespace kgekit
