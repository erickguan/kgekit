#include "translator.h"

namespace kgekit {

TripleIndexTranslator::TripleIndexTranslator(const shared_ptr<vector<string>> entities,
        const shared_ptr<vector<string>> relations,
        const shared_ptr<unordered_map<string, uint32_t>> entity2id,
        const shared_ptr<unordered_map<string, uint32_t>> relation2id) :
            entities_(entities),
            relations_(relations),
            entity2id_(entity2id),
            relation2id_(relation2id)
{
}

optional<string> TripleIndexTranslator::getEntityFromId(uint32_t id)
{
    if (id >= entities_->size()) {
        return {};
    } else {
        return { (*entities_)[id] };
    }
}

optional<string> TripleIndexTranslator::getRelationFromId(uint32_t id)
{
    if (id >= relations_->size()) {
        return {};
    } else {
        return { (*relations_)[id] };
    }
}

optional<uint32_t> TripleIndexTranslator::getIdFromEntity(const string& entity)
{
    const auto entity_id = entity2id_->find(entity);
    if (entity_id != entity2id_->end()) {
        return { entity_id->second };
    } else {
        return {};
    }
}

optional<uint32_t> TripleIndexTranslator::getIdFromRelation(const string& relation)
{
    const auto relation_id = relation2id_->find(relation);
    if (relation_id != relation2id_->end()) {
        return { relation_id->second };
    } else {
        return {};
    }
}

}
