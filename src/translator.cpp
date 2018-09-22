#include "translator.h"

namespace kgekit {

TripleIndexTranslator::TripleIndexTranslator(const shared_ptr<vector<string>> entities,
        const shared_ptr<vector<string>> relations,
        const shared_ptr<unordered_map<string, uint32_t>> entity2id,
        const shared_ptr<unordered_map<string, uint32_t>> relation2id) :
            m_entities(entities),
            m_relations(relations),
            m_entity2id(entity2id),
            m_relation2id(relation2id)
{
}

optional<string> TripleIndexTranslator::getEntityFromId(uint32_t id)
{
    if (id >= m_entities->size()) {
        return {};
    } else {
        return { (*m_entities)[id] };
    }
}

optional<string> TripleIndexTranslator::getRelationFromId(uint32_t id)
{
    if (id >= m_relations->size()) {
        return {};
    } else {
        return { (*m_relations)[id] };
    }
}

optional<uint32_t> TripleIndexTranslator::getIdFromEntity(const string& entity)
{
    const auto entity_id = m_entity2id->find(entity);
    if (entity_id != m_entity2id->end()) {
        return { entity_id->second };
    } else {
        return {};
    }
}

optional<uint32_t> TripleIndexTranslator::getIdFromRelation(const string& relation)
{
    const auto relation_id = m_relation2id->find(relation);
    if (relation_id != m_relation2id->end()) {
        return { relation_id->second };
    } else {
        return {};
    }
}

}
