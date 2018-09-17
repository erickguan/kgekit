#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "indexer.h"

namespace kgekit {

using std::shared_ptr;
using std::vector;
using std::array;

Entity2NumberIndexer::Entity2NumberIndexer(shared_ptr<vector<array<string, 3>>> triples, const string& order) : m_triples(triples), m_order(order)
{
}

unordered_map<string, uint32_t> Entity2NumberIndexer::getEntity2IdMap()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return m_entity2id;
}

unordered_map<string, uint32_t> Entity2NumberIndexer::getRelation2IdMap()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return m_relation2id;
}

bool Entity2NumberIndexer::isIndexBuilt() const
{
    if (m_triples->size() == 0) {
        return true;
    }
    if (m_entities.size() > 0 || m_relations.size() > 0) {
        return true;
    }
    return false;
}

vector<TripleIndex> Entity2NumberIndexer::getIndex()
{
    if (!this->isIndexBuilt()) {
        this->buildIndex();
    }
    return m_indexes;
}

void Entity2NumberIndexer::buildIndex()
{
    internal::assert_triple_order(m_order);

    for (const auto& t : *m_triples) {
        TripleIndex triple_idx;
        uint32_t idx;
        for (auto i = 0; i < m_order.size(); ++i) {
            switch (m_order[i]) {
            case 'h':
                if (m_entity2id.find(t[i]) == m_entity2id.end()) {
                    idx = m_entity2id[t[i]] = m_entities.size();
                    m_entities.push_back(t[i]);
                } else {
                    idx = m_entity2id.at(t[i]);
                }
                triple_idx.head = idx;
                break;
            case 't':
                if (m_entity2id.find(t[i]) == m_entity2id.end()) {
                    idx = m_entity2id[t[i]] = m_entities.size();
                    m_entities.push_back(t[i]);
                } else {
                    idx = m_entity2id.at(t[i]);
                }
                triple_idx.tail = idx;
                break;
            case 'r':
                if (m_relation2id.find(t[i]) == m_relation2id.end()) {
                    idx = m_relation2id[t[i]] = m_relation2id.size();
                    m_relations.push_back(t[i]);
                } else {
                    idx = m_relation2id.at(t[i]);
                }
                triple_idx.relation = idx;
                break;
            }
        }
        m_indexes.push_back(triple_idx);
    }
}

} // namespace kgekit
