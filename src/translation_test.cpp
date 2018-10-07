#include <fstream>
#include <memory>
#include <catch.hpp>

#include "kgekit.h"
#include "indexer.h"
#include "translation.h"

namespace kgekit
{

using namespace Catch;
using namespace std;

class TranslationTestsFixture {
protected:
    EntityNumberIndexer getIndexer()
    {
        std::ifstream f(path);
        std::string h, r, t;
        vector<array<string, 3>> content;
        while (f >> h >> r >> t) {
            content.push_back({h, r, t});
        }
        EntityNumberIndexer indexer(content, "hrt");
        return indexer;
    }
private:
    std::string path = "../../tests/fixtures/triple.txt";
};

TEST_CASE_METHOD(TranslationTestsFixture, "gets entity from id", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(getEntityFromId(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/m/entity2"; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets relation from id", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(getRelationFromId(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/country"; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets entity id from string", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(getIdFromEntity(indexer, "/m/entity1"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets relation id from string", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(getIdFromRelation(indexer, "/produced_by"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

} // namespace kgekit
