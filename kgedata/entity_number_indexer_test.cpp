#include "entity_number_indexer.h"

#include "test_helper.h"
#include <pybind11/pybind11.h>

namespace kgedata
{

using namespace Catch;
using namespace std;

TEST_CASE_METHOD(IndexerTestsFixture, "build index lazyly", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE(indexer->isIndexBuilt() == false);
    indexer->getEntityIdMap();
    REQUIRE(indexer->isIndexBuilt() == true);
}

TEST_CASE_METHOD(IndexerTestsFixture, "returns indexes", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();

    TripleIndex first(0, 0, 1);
    TripleIndex second(1, 1, 2);
    TripleIndex third(0, 2, 2);
    TripleIndex fourth(2, 2, 3);
    TripleIndex fifth(0, 1, 3);
    REQUIRE_THAT(indexer->getIndexes(),
                 Predicate<decltype(indexer->getIndexes())>(
                     [&](const auto& vec) -> bool {
                         return vec->at(0) == first &&
                            vec->at(1) == second &&
                            vec->at(2) == third &&
                            vec->at(3) == fourth &&
                            vec->at(4) == fifth;
                     }));
    REQUIRE_THAT(indexer->getEntityIdMap(),
                 Predicate<decltype(indexer->getEntityIdMap())>(
                     [](const auto& maps) -> bool {
                         return maps->size() == 4 &&
                             maps->at("/m/entity1") == 0 &&
                             maps->at("/m/entity2") == 1 &&
                             maps->at("/m/entity3") == 2 &&
                             maps->at("/m/entity4") == 3; },
                     "entities were inserted."));
    REQUIRE_THAT(indexer->getRelationIdMap(),
        Predicate<decltype(indexer->getRelationIdMap())>(
            [](const auto& maps) -> bool {
                return maps->size() == 3 &&
                    maps->at("/produced_by") == 0 &&
                    maps->at("/country") == 1 &&
                    maps->at("/produced_in") == 2; },
            "relations were inserted."));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets entity from id", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(indexer->getEntityFromId(1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/m/entity2"; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets relation from id", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE(indexer->getEntities()->size() != 0);
    REQUIRE_THAT(indexer->getRelationFromId(1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/country"; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets entity id from string", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(indexer->getIdFromEntity("/m/entity1"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets relation id from string", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(indexer->getIdFromRelation("/produced_by"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

} // namespace kgedata
