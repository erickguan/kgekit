#include <fstream>
#include <iostream>
#include <unordered_set>

#include "kgekit.h"

namespace kgekit {

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

optional<TripleIndex> get_triple_index(const string& line, const string& order, const char sperator)
{
    auto last_pos = 0;
    auto distance = 0;
    TripleIndex triple;

    for (auto p : order) {
        auto pos = line.find(line, last_pos);
        if (pos == std::string::npos) {
            distance = line.size() - last_pos;
        } else {
            distance = pos - last_pos;
        }

        if (distance <= 0) {
            return {};
        }

        string window(&line[last_pos], distance);
        last_pos = pos;

        uint32_t idx = static_cast<u_int32_t>(std::stoul(window));
        switch (p) {
        case 'h':
            triple.head = idx;
            break;
        case 'r':
            triple.relation = idx;
        case 't':
            triple.tail = idx;
        default:
            break;
        }
    }
    return { triple };
}

vector<TripleIndex> read_triple_index(
    const std::string& filename,
    const std::string triple_order,
    const char sperator)
{
    internal::assert_good_file(filename);
    internal::assert_triple_order(triple_order);

    vector<TripleIndex> content;
    std::ifstream fs(filename);
    for (std::string line; std::getline(fs, line); ) {
        if (auto triple = get_triple_index(line, triple_order, sperator)) {
            content.push_back(*triple);
        } else {
            throw std::runtime_error("Can't parse file " + filename + '.');
        }
    }

    return content;
}

} // namespace kgekit
