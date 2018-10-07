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

optional<string> getEntityFromId(const shared_ptr<Indexer>& indexer, uint32_t id);
optional<string> getRelationFromId(const shared_ptr<Indexer>& indexer, uint32_t id);
optional<uint32_t> getIdFromEntity(const shared_ptr<Indexer>& indexer, const string& entity);
optional<uint32_t> getIdFromRelation(const shared_ptr<Indexer>& indexer, const string& relation);

} // namespace kgekit
