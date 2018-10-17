#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "kgekit.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::array;
using std::string;
using std::make_shared;
using std::unordered_map;

class Indexer {
public:
    virtual shared_ptr<unordered_map<string, uint32_t>> getEntityIdMap() = 0;
    virtual shared_ptr<unordered_map<string, uint32_t>> getRelationIdMap() = 0;
    virtual shared_ptr<vector<TripleIndex>> getIndexes() = 0;
    virtual shared_ptr<vector<string>> getEntities() = 0;
    virtual shared_ptr<vector<string>> getRelations() = 0;
};

} // namespace kgekit
