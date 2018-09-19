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

class Entity2NumberIndexer {
public:
    Entity2NumberIndexer(shared_ptr<vector<array<string, 3>>> triples, const string& order);
    shared_ptr<unordered_map<string, uint32_t>> getEntity2IdMap();
    shared_ptr<unordered_map<string, uint32_t>> getRelation2IdMap();
    shared_ptr<vector<TripleIndex>> getIndex();
    shared_ptr<vector<string>> getEntities();
    shared_ptr<vector<string>> getRelations();
    bool isIndexBuilt() const;
private:
    void buildIndex();
    shared_ptr<vector<string>> m_entities;
    shared_ptr<vector<string>> m_relations;
    shared_ptr<unordered_map<string, uint32_t>> m_entity2id;
    shared_ptr<unordered_map<string, uint32_t>> m_relation2id;
    shared_ptr<vector<TripleIndex>> m_indexes;
    shared_ptr<vector<array<string, 3>>> m_triples;
    string m_order;
};

} // namespace kgekit
