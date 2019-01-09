#include "entity_number_indexer.h"

#include <vector>
#include <memory>

namespace py = pybind11;

namespace kgedata {

using std::shared_ptr;
using std::vector;

EntityNumberIndexer::EntityNumberIndexer(const py::list& triples, const string& order) : triples_(triples), order_(order)
{
}

py::dict EntityNumberIndexer::entityIdMap()
{
    auto entity_id = getEntityIdMap();
    py::dict res;
    for (auto const& [key, val] : *entity_id) {
        res[key.c_str()] = val;
    }
    return res;
}

py::dict EntityNumberIndexer::relationIdMap()
{
    auto relation_id = getRelationIdMap();
    py::dict res;
    for (auto const&  [key, val] : *relation_id) {
        res[key.c_str()] = val;
    }
    return res;
}

py::list EntityNumberIndexer::indexes()
{
    auto idxes = getIndexes();
    py::list res;
    for (auto const& idx : *idxes) {
        res.append(idx);
    }
    return res;
}

py::list EntityNumberIndexer::entities()
{
    auto ents = getEntities();
    py::list res;
    for (auto const& idx : *ents) {
        res.append(idx);
    }
    return res;
}

py::list EntityNumberIndexer::relations()
{
    auto rels = getRelations();
    py::list res;
    for (auto const& idx : *rels) {
        res.append(idx);
    }
    return res;
}

shared_ptr<unordered_map<string, uint32_t>> EntityNumberIndexer::getEntityIdMap()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return entity2id_;
}

shared_ptr<unordered_map<string, uint32_t>> EntityNumberIndexer::getRelationIdMap()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return relation2id_;
}

bool EntityNumberIndexer::isIndexBuilt() const
{
    return entities_ != nullptr && relations_ != nullptr && entity2id_ != nullptr && relation2id_ != nullptr;
}

shared_ptr<vector<TripleIndex>> EntityNumberIndexer::getIndexes()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return indexes_;
}

shared_ptr<vector<string>> EntityNumberIndexer::getEntities()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return entities_;
}

shared_ptr<vector<string>> EntityNumberIndexer::getRelations()
{
   if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return relations_;
}

void EntityNumberIndexer::buildIndex()
{
    detail::assert_triple_order(order_);

    entities_ = make_shared<vector<string>>();
    relations_ = make_shared<vector<string>>();
    entity2id_ = make_shared<unordered_map<string, uint32_t>>();
    relation2id_ = make_shared<unordered_map<string, uint32_t>>();
    indexes_ = make_shared<vector<TripleIndex>>();

    for (const auto& t : triples_) {
        auto triple = t.cast<Triple*>();
        TripleIndex triple_idx;
        uint32_t idx;
        for (auto i = 0; i < order_.size(); ++i) {
            switch (order_[i]) {
            case 'h':
                if (entity2id_->find(triple->head) == entity2id_->end()) {
                    idx = (*entity2id_)[triple->head] = entities_->size();
                    entities_->push_back(triple->head);
                } else {
                    idx = entity2id_->at(triple->head);
                }
                triple_idx.head = idx;
                break;
            case 't':
                if (entity2id_->find(triple->tail) == entity2id_->end()) {
                    idx = (*entity2id_)[triple->tail] = entities_->size();
                    entities_->push_back(triple->tail);
                } else {
                    idx = entity2id_->at(triple->tail);
                }
                triple_idx.tail = idx;
                break;
            case 'r':
                if (relation2id_->find(triple->relation) == relation2id_->end()) {
                    idx = (*relation2id_)[triple->relation] = relations_->size();
                    relations_->push_back(triple->relation);
                } else {
                    idx = relation2id_->at(triple->relation);
                }
                triple_idx.relation = idx;
                break;
            }
        }
        indexes_->push_back(triple_idx);
    }
}

optional<string> EntityNumberIndexer::getEntityFromId(uint32_t id)
{
    auto entities = getEntities();
    if (id >= entities->size()) {
        return {};
    } else {
        return { (*entities)[id] };
    }
}

optional<string> EntityNumberIndexer::getRelationFromId(uint32_t id)
{
    auto relations = getRelations();
    if (id >= relations->size()) {
        return {};
    } else {
        return { (*relations)[id] };
    }
}

optional<uint32_t> EntityNumberIndexer::getIdFromEntity(const string& entity)
{
    auto entity2id = getEntityIdMap();
    const auto entity_id = entity2id->find(entity);
    if (entity_id != entity2id->end()) {
        return { entity_id->second };
    } else {
        return {};
    }
}

optional<uint32_t> EntityNumberIndexer::getIdFromRelation(const string& relation)
{
    auto relation2id = getRelationIdMap();
    const auto relation_id = relation2id->find(relation);
    if (relation_id != relation2id->end()) {
        return { relation_id->second };
    } else {
        return {};
    }
}


} // namespace kgedata
