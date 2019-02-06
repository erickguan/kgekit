#include "test_helper.h"
#include "kgedata.h"
#include "ranker.h"

#include <unordered_set>

namespace kgedata
{

using namespace Catch;

TEST_CASE("rank_raw_ascending", "[impl]")
{
    float a[] = {0.001, 0.002, 0.003, 0.004, 0.005};
    std::unordered_set<int64_t> filters;
    filters.insert(3);
    auto r1 = *rank_raw(&a[0], &a[5], 1, filters, true);
    REQUIRE(r1.first == 2);
    REQUIRE(r1.second == 2);
    auto r2 = *rank_raw(&a[0], &a[5], 4, filters, true);
    REQUIRE(r2.first == 5);
    REQUIRE(r2.second == 4);

    auto r3 = *rank_raw(&a[0], &a[4], 2, filters, true);
    REQUIRE(r3.first == 3);
    REQUIRE(r3.second == 3);

    auto r4 = rank_raw(&a[0], &a[0], 2, filters);
    REQUIRE(r4.has_value() == false);
}

TEST_CASE("rank_raw_descending", "[impl]")
{
    float a[] = {0.001, 0.002, 0.003, 0.004, 0.005};
    std::unordered_set<int64_t> filters;
    filters.insert(2);
    auto r1 = *rank_raw(&a[0], &a[5], 1, filters, false);
    REQUIRE(r1.first == 4);
    REQUIRE(r1.second == 3);
    auto r2 = *rank_raw(&a[0], &a[5], 4, filters, false);
    REQUIRE(r2.first == 1);
    REQUIRE(r2.second == 1);

    auto r3 = *rank_raw(&a[0], &a[4], 2, filters, false);
    REQUIRE(r3.first == 2);
    REQUIRE(r3.second == 2);
}

} // namespace kgedata
