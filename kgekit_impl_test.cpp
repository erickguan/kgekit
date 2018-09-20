#define CATCH_CONFIG_MAIN

#include <fstream>
#include <experimental/filesystem>
#include <cstdio>
#include <catch.hpp>
#include "kgekit.h"

namespace fs = std::experimental::filesystem;

namespace kgekit
{

using namespace Catch;

namespace internal
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


struct FileFolderTestsFixture {
    FileFolderTestsFixture()
    {
        std::ofstream f1;
        f1.open(local);
        fs::create_directory(folder);
        std::ofstream f2;
        f2.open(file_in_folder);
    }
    ~FileFolderTestsFixture()
    {
        fs::remove(local);
        fs::remove_all(folder);
    }
    fs::path local = "local.txt";
    fs::path folder = "folder";
    fs::path file_in_folder = folder / "file.txt";
    fs::path wrong_file = "wrong.txt";
};

TEST_CASE_METHOD(FileFolderTestsFixture, "assert_good_file", "[io]")
{
    REQUIRE_NOTHROW(assert_good_file(local));
    REQUIRE_NOTHROW(assert_good_file(file_in_folder));
    REQUIRE_THROWS_AS(assert_good_file(folder), std::invalid_argument);   // throw for folder
    REQUIRE_THROWS_AS(assert_good_file(wrong_file), std::invalid_argument);    // throw for no file
}

} // namespace internal


TEST_CASE("get_triple_index", "[parsing]")
{
    REQUIRE_THAT(get_triple_index("1\t2\t3", "hrt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return t->head == 1 && t->relation == 2 && t->tail == 3; }, "gets the triple"));
    REQUIRE_THAT(get_triple_index("1\t3\t2", "htr", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return t->head == 1 && t->relation == 2 && t->tail == 3; }, "gets the triple"));
    REQUIRE_THAT(get_triple_index("1\t2\t3", "hrt", ' '), // can be replaced by t.has_value()
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return if (t) { return true; } else { return false; } }, "return null when wrong separator doesn't return value"));
    REQUIRE_THAT(get_triple_index("1\t2\t3", "htt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return if (t) { return false; } else { return true; } }, "return null when wrong order for the triple index"));
    REQUIRE_THAT(get_triple_index("1\t3", "hrt", '\t'),
                 Predicate<optional<TripleIndex>>([] (const auto& t) -> bool { return if (t) { return false; } else { return true; } }, "return null when wrong content for getting the triple index"));
}

TEST_CASE("read_triple_index", "[parsing][io]")
{
    auto index = "../tests/fixtures/triple_index.txt";
    TripleIndex first { 1, 2, 3 };
    TripleIndex second { 9, 1, 3 };
    REQUIRE_THAT(read_triple_index(index, "hrt", ' '), VectorContains(first) && VectorContains(second));
}

} // namespace kgekit
