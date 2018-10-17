#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "kgekit.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::array;
using std::unordered_map;

class Indexer;

optional<string> get_entity_from_id(const shared_ptr<Indexer>& indexer, uint32_t id);
optional<string> get_relation_from_id(const shared_ptr<Indexer>& indexer, uint32_t id);
optional<uint32_t> get_id_from_entity(const shared_ptr<Indexer>& indexer, const string& entity);
optional<uint32_t> get_id_from_relation(const shared_ptr<Indexer>& indexer, const string& relation);

} // namespace kgekit
