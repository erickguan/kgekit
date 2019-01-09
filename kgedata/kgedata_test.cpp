#define CATCH_CONFIG_MAIN

#include "test_helper.h"
#include "kgedata.h"

namespace kgedata
{

using namespace Catch;

namespace detail
{

TEST_CASE("assert_triple_order", "[io]")
{
    REQUIRE_NOTHROW(assert_triple_order("hrt"));
    REQUIRE_NOTHROW(assert_triple_order("rth"));                             // takes different shape
    REQUIRE_THROWS_AS(assert_triple_order("h rt"), std::invalid_argument);   // no blanks
    REQUIRE_THROWS_AS(assert_triple_order("hrr"), std::invalid_argument);    // no repetitive character
    REQUIRE_THROWS_AS(assert_triple_order("hft"), std::invalid_argument);    // no random character
    REQUIRE_THROWS_AS(assert_triple_order("hrtwes"), std::invalid_argument); // no too long order
    REQUIRE_THROWS_AS(assert_triple_order("h"), std::invalid_argument);      // not too short order
}

} // namespace detail

TEST_CASE("TripleIndex", "[data]")
{
    TripleIndex index;
    index.head = 1;
    index.relation = 2;
    index.tail = 3;
    REQUIRE(TripleIndex(1, 2, 3) == index);
}

TEST_CASE("Triple", "[data]")
{
    Triple triple;
    triple.head = "head";
    triple.relation = "relation";
    triple.tail = "tail";
    REQUIRE(Triple("head", "relation", "tail") == triple);
}

TEST_CASE("get_triple_index", "[parsing]")
{
    REQUIRE_THAT(get_triple_index("1\t2\t3", "hrt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return t->head == 1 && t->relation == 2 && t->tail == 3; }, "gets the triple"));
    REQUIRE_THAT(get_triple_index("1\t3\t2", "htr", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return t->head == 1 && t->relation == 2 && t->tail == 3; }, "gets the triple"));
    REQUIRE_THAT(get_triple_index("1\t2\t3", "hrt", ' '), // can be replaced by t.has_value()
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong separator doesn't return value"));
    REQUIRE_THAT(get_triple_index("1\t2\t3", "htt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong order for the triple index"));
    REQUIRE_THAT(get_triple_index("1\t3", "hrt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong content for getting the triple index"));
    REQUIRE_THAT(get_triple_index("1\t2\t3", "htt", '\t', true),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return t->head == 1 && t->relation == -1 && t->tail == 3; }, "tries to get the triple if skipping flag"));
}


TEST_CASE("get_triple", "[parsing]")
{
    REQUIRE_THAT(get_triple("Essential myocardial infarction", "hrt", ' '),
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { return t->head == "Essential" && t->relation == "myocardial" && t->tail == "infarction"; }, "gets the triple"));
    REQUIRE_THAT(get_triple("Essential infarction myocardial", "htr", ' '),
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { return t->head == "Essential" && t->relation == "myocardial" && t->tail == "infarction"; }, "gets the triple"));
    REQUIRE_THAT(get_triple("1\t2\t3", "hrt", ' '), // can be replaced by t.has_value()
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong separator doesn't return value"));
    REQUIRE_THAT(get_triple("1\t2\t3", "htt", '\t'),
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong order for the triple index"));
    REQUIRE_THAT(get_triple("1\t3", "hrt", '\t'),
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { if (t) { return false; } else { return true; } }, "return null when wrong content for getting the triple index"));
    REQUIRE_THAT(get_triple("Essential myocardial infarction", "htt", ' ', true),
                 Predicate<optional<Triple>>([] (const auto& t) -> bool { return t->head == "Essential" && t->relation == "" && t->tail == "infarction";}, "tries to get the triple if skipping flag"));
}

} // namespace kgedata
