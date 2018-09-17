#include <fstream>
#include <memory>
#include <catch.hpp>

#include "kgekit.h"
#include "indexer.h"

namespace kgekit
{

using namespace Catch;
using namespace std;

class IndexerTestsFixture {
public:
    IndexerTestsFixture()
    {
        std::ifstream f(path);

        std::string h, r, t;
        while (f >> h >> r >> t) {
            content.push_back({h, r, t});
        }
    }
protected:
    Entity2NumberIndexer getEntity2NumberIndexer()
    {
        return Entity2NumberIndexer(std::make_shared<std::vector<std::array<std::string, 3>>>(content), "hrt");
    }
private:
    std::string path = "../tests/fixtures/triple.txt";
    std::vector<std::array<std::string, 3>> content;
};

TEST_CASE_METHOD(IndexerTestsFixture, "build index lazyly", "[indexing]")
{
    auto indexer = getEntity2NumberIndexer();
    REQUIRE(indexer.isIndexBuilt() == false);
    indexer.getEntity2IdMap();
    REQUIRE(indexer.isIndexBuilt() == true);
}

TEST_CASE_METHOD(IndexerTestsFixture, "returns indexes", "[indexing]")
{
    auto indexer = getEntity2NumberIndexer();

    TripleIndex first { 0, 0, 1 };
    TripleIndex second { 1, 1, 2 };
    TripleIndex third { 0, 2, 2 };
    REQUIRE_THAT(indexer.getIndex(), VectorContains(first) && VectorContains(second) && VectorContains(third));
    REQUIRE_THAT(indexer.getEntity2IdMap(),
                 Predicate<decltype(indexer.getEntity2IdMap())>(
                     [](auto& maps) -> bool {
                         return maps.size() == 3 &&
                             maps.at("/m/entity1") == 0 &&
                             maps.at("/m/entity2") == 1 &&
                             maps.at("/m/entity3") == 2; },
                     "entities were inserted."));
    REQUIRE_THAT(indexer.getRelation2IdMap(),
        Predicate<decltype(indexer.getRelation2IdMap())>(
            [](auto& maps) -> bool {
                return maps.size() == 3 &&
                    maps.at("/produced_by") == 0 &&
                    maps.at("/country") == 1 &&
                    maps.at("/produced_in") == 2; },
            "relations were inserted."));
}

} // namespace kgekit
