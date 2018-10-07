#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <range/v3/all.hpp>
#include <range/v3/view/split.hpp>

#include "kgekit.h"

namespace kgekit {
using std::vector;
namespace internal {

void assert_good_file(fs::path filename)
{
    try {
        if (!fs::exists(filename) || !fs::is_regular_file(filename)) {
            throw std::invalid_argument("File is not valid. Maybe wrong path or is a directory.");
        }
    } catch (const fs::filesystem_error& e) {
        throw std::invalid_argument("File is not valid. Maybe wrong path or is a directory.");
    }
}

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

optional<TripleIndex> get_triple_index(const string& line, const string& order, const char delimiter)
{
    try {
        internal::assert_triple_order(order);
    } catch (const std::invalid_argument& e) {
        return {};
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

vector<TripleIndex> read_triple_index(
    const std::string& filename,
    const std::string triple_order,
    const char delimiter)
{
    internal::assert_good_file(filename);
    internal::assert_triple_order(triple_order);

    vector<TripleIndex> content;
    std::ifstream fs(filename);
    for (std::string line; std::getline(fs, line); ) {
        if (auto triple = get_triple_index(line, triple_order, delimiter)) {
            content.push_back(*triple);
        } else {
            throw std::runtime_error("Can't parse file " + filename + '.');
        }
    }

    return content;
}

vector<TripleIndex> read_triple(
    const std::string& filename,
    const std::string triple_order,
    const char delimiter)
{
    internal::assert_good_file(filename);
    internal::assert_triple_order(triple_order);

    vector<TripleIndex> content;
    std::ifstream fs(filename);
    for (std::string line; std::getline(fs, line); ) {
        if (auto triple = get_triple_index(line, triple_order, delimiter)) {
            content.push_back(*triple);
        } else {
            throw std::runtime_error("Can't parse file " + filename + '.');
        }
    }

    return content;
}

} // namespace kgekit
