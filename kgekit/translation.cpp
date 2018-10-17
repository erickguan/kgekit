#include "translation.h"
#include "indexer.h"

namespace kgekit {

optional<string> get_entity_from_id(const shared_ptr<Indexer>& indexer, uint32_t id)
{
    auto entities = indexer->getEntities();
    if (id >= entities->size()) {
        return {};
    } else {
        return { (*entities)[id] };
    }
}

optional<string> get_relation_from_id(const shared_ptr<Indexer>& indexer, uint32_t id)
{
    auto relations = indexer->getRelations();
    if (id >= relations->size()) {
        return {};
    } else {
        return { (*relations)[id] };
    }
}

optional<uint32_t> get_id_from_entity(const shared_ptr<Indexer>& indexer, const string& entity)
{
    auto entity2id = indexer->getEntityIdMap();
    const auto entity_id = entity2id->find(entity);
    if (entity_id != entity2id->end()) {
        return { entity_id->second };
    } else {
        return {};
    }
}

optional<uint32_t> get_id_from_relation(const shared_ptr<Indexer>& indexer, const string& relation)
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
