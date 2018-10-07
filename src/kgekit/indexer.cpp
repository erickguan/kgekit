#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "indexer.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::array;

EntityNumberIndexer::EntityNumberIndexer(const vector<Triple>& triples, const string& order) : triples_(make_shared<vector<Triple>>(triples)), order_(order)
{
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
    return triples_ != nullptr && entities_ != nullptr && relations_ != nullptr && entity2id_ != nullptr && relation2id_ != nullptr;
}

shared_ptr<vector<TripleIndex>> EntityNumberIndexer::getIndex()
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
    internal::assert_triple_order(order_);

    entities_ = make_shared<vector<string>>();
    relations_ = make_shared<vector<string>>();
    entity2id_ = make_shared<unordered_map<string, uint32_t>>();
    relation2id_ = make_shared<unordered_map<string, uint32_t>>();
    indexes_ = make_shared<vector<TripleIndex>>();

    for (const auto& t : *triples_) {
        TripleIndex triple_idx;
        uint32_t idx;
        for (auto i = 0; i < order_.size(); ++i) {
            switch (order_[i]) {
            case 'h':
                if (entity2id_->find(t.head) == entity2id_->end()) {
                    idx = (*entity2id_)[t.head] = entities_->size();
                    entities_->push_back(t.head);
                } else {
                    idx = entity2id_->at(t.head);
                }
                triple_idx.head = idx;
                break;
            case 't':
                if (entity2id_->find(t.tail) == entity2id_->end()) {
                    idx = (*entity2id_)[t.tail] = entities_->size();
                    entities_->push_back(t.tail);
                } else {
                    idx = entity2id_->at(t.tail);
                }
                triple_idx.tail = idx;
                break;
            case 'r':
                if (relation2id_->find(t.relation) == relation2id_->end()) {
                    idx = (*relation2id_)[t.relation] = relation2id_->size();
                    relations_->push_back(t.relation);
                } else {
                    idx = relation2id_->at(t.relation);
                }
                triple_idx.relation = idx;
                break;
            }
        }
        indexes_->push_back(triple_idx);
    }
}

} // namespace kgekit
