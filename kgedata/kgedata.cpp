#include "kgedata.h"

#include <vector>
#include <unordered_set>
#include <type_traits>

#include <boost/algorithm/string.hpp>

namespace kgedata {

using std::vector;

namespace detail {

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

template<typename T>
inline typename std::enable_if<!std::is_scalar_v<T>, const T&>::type
_get_triple_element(const string& element) { return element; }

template<typename T>
inline typename std::enable_if<std::is_scalar_v<T>, T>::type
_get_triple_element(const string& element) { return static_cast<T>(std::stol(element)); }

template<typename TripleType>
inline optional<TripleType> _get_triple_impl(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order)
{
    if (!skip_checking_order) {
        try {
            detail::assert_triple_order(order);
        } catch (const std::invalid_argument& e) {
            return {};
        }
    }
    decltype(line.find(delimiter)) last_pos = 0;
    TripleType triple;

    vector<string> splitted;
    boost::split(splitted, line, [&](char c) { return c == delimiter; });

    for (auto i = 0; i < order.size(); ++i) {
        if (i >= splitted.size()) { return {}; }

        auto idx = _get_triple_element<decltype(triple.head)>(splitted[i]);
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

} // namespace detail

optional<TripleIndex> get_triple_index(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order)
{
    return detail::_get_triple_impl<TripleIndex>(line, order, delimiter, skip_checking_order);
}


optional<Triple> get_triple(
    const string& line,
    const string& order,
    const char delimiter,
    bool skip_checking_order)
{
    return detail::_get_triple_impl<Triple>(line, order, delimiter, skip_checking_order);
}

} // namespace kgedata
