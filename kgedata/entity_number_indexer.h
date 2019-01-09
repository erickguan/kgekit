#pragma once

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include <pybind11/pytypes.h>
#include <pybind11/cast.h>
#include <boost/core/noncopyable.hpp>

#include "kgedata.h"

namespace kgedata {

using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;
using std::unordered_map;

class EntityNumberIndexer: private boost::noncopyable {
public:
    EntityNumberIndexer(const pybind11::list& triples, const string& order);
    pybind11::dict entityIdMap();
    pybind11::dict relationIdMap();
    pybind11::list indexes();
    pybind11::list entities();
    pybind11::list relations();
    shared_ptr<unordered_map<string, uint32_t>> getEntityIdMap();
    shared_ptr<unordered_map<string, uint32_t>> getRelationIdMap();
    shared_ptr<vector<TripleIndex>> getIndexes();
    shared_ptr<vector<string>> getEntities();
    shared_ptr<vector<string>> getRelations();
    optional<string> getEntityFromId(uint32_t id);
    optional<string> getRelationFromId(uint32_t id);
    optional<uint32_t> getIdFromEntity(const string& entity);
    optional<uint32_t> getIdFromRelation(const string& relation);
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

} // namespace kgedata
