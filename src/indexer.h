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

class EntityNumberIndexer {
public:
    EntityNumberIndexer(shared_ptr<vector<array<string, 3>>> triples, const string& order);
    shared_ptr<unordered_map<string, uint32_t>> getEntityIdMap();
    shared_ptr<unordered_map<string, uint32_t>> getRelationIdMap();
    shared_ptr<vector<TripleIndex>> getIndex();
    shared_ptr<vector<string>> getEntities();
    shared_ptr<vector<string>> getRelations();
    bool isIndexBuilt() const; // public for testing
private:
    void buildIndex();
    shared_ptr<vector<string>> entities_;
    shared_ptr<vector<string>> relations_;
    shared_ptr<unordered_map<string, uint32_t>> entity2id_;
    shared_ptr<unordered_map<string, uint32_t>> relation2id_;
    shared_ptr<vector<TripleIndex>> indexes_;
    shared_ptr<vector<array<string, 3>>> triples_;
    string order_;
};

} // namespace kgekit
