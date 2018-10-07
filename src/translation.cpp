#include "translation.h"
#include "indexer.h"

namespace kgekit {

optional<string> getEntityFromId(const shared_ptr<Indexer>& indexer, uint32_t id)
{
    auto entities = indexer->getEntities();
    if (id >= entities->size()) {
        return {};
    } else {
        return { (*entities)[id] };
    }
}

optional<string> getRelationFromId(const shared_ptr<Indexer>& indexer, uint32_t id)
{
    auto relations = indexer->getRelations();
    if (id >= relations->size()) {
        return {};
    } else {
        return { (*relations)[id] };
    }
}

optional<uint32_t> getIdFromEntity(const shared_ptr<Indexer>& indexer, const string& entity)
{
    auto entity2id = indexer->getEntityIdMap();
    const auto entity_id = entity2id->find(entity);
    if (entity_id != entity2id->end()) {
        return { entity_id->second };
    } else {
        return {};
    }
}

optional<uint32_t> getIdFromRelation(const shared_ptr<Indexer>& indexer, const string& relation)
{
    auto relation2id = indexer->getRelationIdMap();
    const auto relation_id = relation2id->find(relation);
    if (relation_id != relation2id->end()) {
        return { relation_id->second };
    } else {
        return {};
    }
}

}
