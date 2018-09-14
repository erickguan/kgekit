#define CATCH_CONFIG_MAIN

#include <fstream>
#include <experimental/filesystem>
#include <cstdio>
#include <catch.hpp>
#include "kgekit.h"

namespace fs = std::experimental::filesystem;

namespace kgekit
{
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
} // namespace kgekit
