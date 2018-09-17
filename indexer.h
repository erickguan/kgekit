#pragma once

#include <array>
#include <vector>
#include <memory>
#include <unordered_map>

#include "kgekit.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::array;
using std::unordered_map;

class Entity2NumberIndexer {
public:
    Entity2NumberIndexer(shared_ptr<vector<array<string, 3>>> triples, const string& order);
    unordered_map<string, uint32_t> getEntity2IdMap();
    unordered_map<string, uint32_t> getRelation2IdMap();
    vector<TripleIndex> getIndex();
    bool isIndexBuilt() const;
private:
    void buildIndex();
    vector<string> m_entities;
    vector<string> m_relations;
    unordered_map<string, uint32_t> m_entity2id;
    unordered_map<string, uint32_t> m_relation2id;
    vector<TripleIndex> m_indexes;
    shared_ptr<vector<array<string, 3>>> m_triples;
    string m_order;
};

} // namespace kgekit
