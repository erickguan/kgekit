#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <pybind11/pytypes.h>
#include <pybind11/cast.h>
#include "kgekit.h"
#include "indexer.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;
using std::unordered_map;

class EntityNumberIndexer : public Indexer {
public:
    EntityNumberIndexer(const pybind11::list& triples, const string& order);
    pybind11::dict entityIdMap();
    pybind11::dict relationIdMap();
    pybind11::list indexes();
    pybind11::list entities();
    pybind11::list relations();
    shared_ptr<unordered_map<string, uint32_t>> getEntityIdMap() override;
    shared_ptr<unordered_map<string, uint32_t>> getRelationIdMap() override;
    shared_ptr<vector<TripleIndex>> getIndexes() override;
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
    pybind11::list triples_;
    string order_;
};

} // namespace kgekit
