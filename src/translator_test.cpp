#include <fstream>
#include <memory>
#include <catch.hpp>

#include "kgekit.h"
#include "indexer.h"
#include "translator.h"

namespace kgekit
{

using namespace Catch;
using namespace std;

class TranslatorTestsFixture {
protected:
    TripleIndexTranslator getTripleIndexTranslator()
    {
        std::ifstream f(path);
        std::string h, r, t;
        vector<array<string, 3>> content;
        while (f >> h >> r >> t) {
            content.push_back({h, r, t});
        }
        EntityNumberIndexer indexer(content, "hrt");
        TripleIndexTranslator translator(indexer.getEntities(), indexer.getRelations(), indexer.getEntityIdMap(), indexer.getRelationIdMap());
        return translator;
    }
private:
    std::string path = "../../tests/fixtures/triple.txt";
};

TEST_CASE_METHOD(TranslatorTestsFixture, "gets entity from id", "[indexing]")
{
    auto translator = getTripleIndexTranslator();
    REQUIRE_THAT(translator.getEntityFromId(1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/m/entity2"; }));
}

TEST_CASE_METHOD(TranslatorTestsFixture, "gets relation from id", "[indexing]")
{
    auto translator = getTripleIndexTranslator();
    REQUIRE_THAT(translator.getRelationFromId(1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/country"; }));
}

TEST_CASE_METHOD(TranslatorTestsFixture, "gets entity id from string", "[indexing]")
{
    auto translator = getTripleIndexTranslator();
    REQUIRE_THAT(translator.getIdFromEntity("/m/entity1"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

TEST_CASE_METHOD(TranslatorTestsFixture, "gets relation id from string", "[indexing]")
{
    auto translator = getTripleIndexTranslator();
    REQUIRE_THAT(translator.getIdFromRelation("/produced_by"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

} // namespace kgekit
