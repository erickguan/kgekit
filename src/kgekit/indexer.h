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
    virtual shared_ptr<vector<TripleIndex>> getIndex() = 0;
    virtual shared_ptr<vector<string>> getEntities() = 0;
    virtual shared_ptr<vector<string>> getRelations() = 0;
};

class EntityNumberIndexer : public Indexer {
public:
    EntityNumberIndexer(const vector<array<string, 3>>& triples, const string& order);
    shared_ptr<unordered_map<string, uint32_t>> getEntityIdMap() override;
    shared_ptr<unordered_map<string, uint32_t>> getRelationIdMap() override;
    shared_ptr<vector<TripleIndex>> getIndex() override;
    shared_ptr<vector<string>> getEntities() override;
    shared_ptr<vector<string>> getRelations() override;
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
