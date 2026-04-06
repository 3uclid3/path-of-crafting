#include <poc/item.hpp>

#include <doctest/doctest.h>

namespace poc { namespace {

TEST_CASE("item::is_blob: returns true for any item blobs")
{
    CHECK(item::is_blob("Item Class: Belts\nRarity: Normal"));
    CHECK(item::is_blob("Item Class: Belts\nRarity: Magic"));
    CHECK(item::is_blob("Item Class: Belts\nRarity: Rare"));
    CHECK(item::is_blob("Item Class: Belts\nRarity: Unique"));
}

TEST_CASE("item::is_blob: returns false for non-item blobs")
{
    CHECK_FALSE(item::is_blob("This is not an item blob"));
}

TEST_CASE("item::is_magic_blob: returns true for magic item blobs")
{
    CHECK(item::is_magic_blob("Item Class: Belts\nRarity: Magic"));
}

TEST_CASE("item::is_magic_blob: returns false for non-magic item blobs")
{
    CHECK_FALSE(item::is_magic_blob("Item Class: Belts\nRarity: Rare"));
}

TEST_CASE("item::is_magic_blob: returns false for non-item blobs")
{
    CHECK_FALSE(item::is_magic_blob("This is not an item blob"));
}

TEST_CASE("item::parse_name: extracts name from magic item blob")
{
    std::string_view blob = "Item Class: Belts\nRarity: Magic\nSome Other Info\n";
    CHECK(item::parse_name(blob) == "Some Other Info");
}

TEST_CASE("item::parse_name: extracts name from rare item blob")
{
    std::string_view blob = "Item Class: Belts\nRarity: Rare\nSome Other Info\n";
    CHECK(item::parse_name(blob) == "Some Other Info");
}

}} // namespace poc