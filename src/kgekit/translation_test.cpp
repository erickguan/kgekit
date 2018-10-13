#include "translation.h"

#include "test_helper.h"
#include "entity_number_indexer.h"
#include "kgekit.h"
#include <pybind11/pybind11.h>

namespace kgekit
{

using namespace Catch;
using namespace std;

TEST_CASE_METHOD(IndexerTestsFixture, "gets entity from id", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(get_entity_from_id(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/m/entity2"; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets relation from id", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE(indexer->getEntities()->size() != 0);
    REQUIRE_THAT(get_relation_from_id(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/country"; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets entity id from string", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(get_id_from_entity(indexer, "/m/entity1"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

TEST_CASE_METHOD(IndexerTestsFixture, "gets relation id from string", "[indexing]")
{
    auto indexer = getIndexer<EntityNumberIndexer>();
    REQUIRE_THAT(get_id_from_relation(indexer, "/produced_by"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

} // namespace kgekit
