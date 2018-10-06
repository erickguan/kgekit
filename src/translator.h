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

class TripleIndexTranslator {
public:
    TripleIndexTranslator(const shared_ptr<vector<string>> entities,
        const shared_ptr<vector<string>> relations,
        const shared_ptr<unordered_map<string, uint32_t>> entity2id,
        const shared_ptr<unordered_map<string, uint32_t>> relation2id);
    optional<string> getEntityFromId(uint32_t id);
    optional<string> getRelationFromId(uint32_t id);
    optional<uint32_t> getIdFromEntity(const string& entity);
    optional<uint32_t> getIdFromRelation(const string& relation);
private:
    shared_ptr<vector<string>> entities_;
    shared_ptr<vector<string>> relations_;
    shared_ptr<unordered_map<string, uint32_t>> entity2id_;
    shared_ptr<unordered_map<string, uint32_t>> relation2id_;
};

} // namespace kgekit
