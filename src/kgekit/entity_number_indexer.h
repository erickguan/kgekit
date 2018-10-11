#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "kgekit.h"
#include "indexer.h"

#include <pybind11/pytypes.h>

namespace py = pybind11;

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;
using std::unordered_map;

class EntityNumberIndexer : public Indexer {
public:
    EntityNumberIndexer(const pybind11::list& triples, const string& order);
    py::dict entityIdMap();
    py::dict relationIdMap();
    py::list indexes();
    py::list entities();
    py::list relations();
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
