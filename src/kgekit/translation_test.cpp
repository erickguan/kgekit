#include <fstream>
#include <memory>
#include <catch.hpp>

#include "kgekit.h"
#include "entity_number_indexer.h"
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
        py::list content;
        while (f >> h >> r >> t) {
            content.append(Triple({h, r, t}));
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
    REQUIRE_THAT(get_entity_from_id(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/m/entity2"; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets relation from id", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(get_relation_from_id(indexer, 1),
                 Predicate<optional<string>>(
                     [](const auto& res) { return *res == "/country"; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets entity id from string", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(get_id_from_entity(indexer, "/m/entity1"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

TEST_CASE_METHOD(TranslationTestsFixture, "gets relation id from string", "[indexing]")
{
    auto indexer = make_shared<EntityNumberIndexer>(getIndexer());
    REQUIRE_THAT(get_id_from_relation(indexer, "/produced_by"),
                 Predicate<optional<uint32_t>>(
                     [](const auto& res) { return *res == 0; }));
}

} // namespace kgekit
